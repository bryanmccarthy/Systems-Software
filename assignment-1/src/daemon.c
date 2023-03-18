/*
 *
 * At 11:30 pm , there will be a check for the xml files that have been uploaded or 
 * files that have not been upladed
 *
 * At 1:00 am, the xml reports will be backed up from xml_upload_directory to 
 * dashboard_directory
 *
 * Directories are locked during transfer / backup
 * 
 * Kill -2 signal will enable the daemon to transfer / backup at any given time
 *
 * */

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <syslog.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>
#include "daemon_task.h"
#include <signal.h>
#include <sys/inotify.h>
#include <pwd.h>

#define EVENT_SIZE (sizeof(struct inotify_event))
#define EVENT_BUF_LEN (1024 * (EVENT_SIZE + 16))

int main() {
    FILE *systemlogs;
    FILE *log_file;
    time_t now;

    struct tm backup_time;
    time(&now);
    backup_time = *localtime(&now);
    // backup_time.tm_sec += 30; // TODO: remove
    backup_time.tm_hour = 1; 
    backup_time.tm_min = 0; 
    backup_time.tm_sec = 0;
    
    struct tm check_uploads_time;
    time(&now);
    check_uploads_time = *localtime(&now);
    // check_uploads_time.tm_sec += 15; // TODO: remove
    check_uploads_time.tm_hour = 23; 
    check_uploads_time.tm_min = 30;
    check_uploads_time.tm_sec = 0;

    // Create a child process      
    int pid = fork();
 
    if (pid > 0) {
        printf("Parent process: %d\n", getpid());
        sleep(3);
        exit(EXIT_SUCCESS);
    } else if (pid == 0) {
        // Child process
        printf("Forked: Child process: %d, Childs parent: %d\n", getpid(), getppid());
      
        // Elevate the orphan process to session leader to lose controlling TTY
        if (setsid() < 0) { 
            exit(EXIT_FAILURE); 
        }

        // We could fork here again , just to guarantee that the process is not a session leader
        int pid = fork();
        if (pid > 0) {
            printf("Parent process: %d\n", getpid());
            exit(EXIT_SUCCESS);
        } else {
            printf("Forked again: Child process: %d, Childs parent: %d\n", getpid(), getppid());

            // Set the file mode creation mask to 0
            umask(0);

            // Change dir to root
            if (chdir("/") < 0) {
                systemlogs = fopen(SYSTEM_LOGS, "a+");
                fprintf(systemlogs, "ERROR: daemon.c : chdir() failed");
                fclose(systemlogs);
                exit(EXIT_FAILURE);
            }

            // Close all open file descriptors
            int maxfd = sysconf(_SC_OPEN_MAX);
            for (int fd = 0; fd < maxfd; fd++) {
                close(fd);
            }

            // Signal Handler
            if (signal(SIGINT, sig_handler) == SIG_ERR) {
                systemlogs = fopen(SYSTEM_LOGS, "a+");
                fprintf(systemlogs, "ERROR: daemon.c : SIG_ERR RECEIVED");
                fclose(systemlogs);
            }

            // Get login name
            struct passwd *user;

            uid_t uid = getuid();
            user = getpwuid(uid);
            if (user == NULL) {
                perror("getpwuid");
                exit(1);
            }

            int length, i = 0;
            char buffer[EVENT_BUF_LEN];

            // Watch for changes in the UPLOAD_DIR
            int fd = inotify_init();
            if (fd < 0) {
              systemlogs = fopen(SYSTEM_LOGS, "a+");
              fprintf(systemlogs, "ERROR: daemon.c : inotify_init() failed");
              fclose(systemlogs);
            }

            int wd = inotify_add_watch(fd, UPLOAD_DIR, IN_ALL_EVENTS);
            if (wd < 0) {
              systemlogs = fopen(SYSTEM_LOGS, "a+");
              fprintf(systemlogs, "ERROR: daemon.c : inotify_add_watch() failed");
              fclose(systemlogs);
            }
	
            while(1) {
                sleep(1);

                length = read(fd, buffer, EVENT_BUF_LEN);
                if (length < 0) {
                    systemlogs = fopen(SYSTEM_LOGS, "a+");
                    fprintf(systemlogs, "ERROR: daemon.c : read() failed");
                    fclose(systemlogs);
                }

                i = 0;
                while (i < length) {
                    struct inotify_event *event = (struct inotify_event *)&buffer[i];
                    time_t event_time = time(NULL) - (length - i) / (EVENT_SIZE + event->len) - 1;
                    char *timestamp = ctime(&event_time);
                    timestamp[strlen(timestamp) - 1] = '\0';

                    if (event->mask & IN_CREATE) {
                        log_file = fopen(LOG_FILE, "a+");
                        fprintf(log_file, "File %s created by %s. Time: %s\n", event->name, user->pw_name, timestamp);
                        fclose(log_file);
                    }
                    if (event->mask & IN_DELETE) {
                        log_file = fopen(LOG_FILE, "a+");
                        fprintf(log_file, "File %s deleted by %s. Time: %s\n", event->name, user->pw_name, timestamp);
                        fclose(log_file);
                    }
                    if (event->mask & IN_MODIFY) {
                        log_file = fopen(LOG_FILE, "a+");
                        fprintf(log_file, "File %s modified by %s. Time: %s\n", event->name, user->pw_name, timestamp);
                        fclose(log_file);
                    }
                    i += EVENT_SIZE + event->len;
                }

                systemlogs = fopen(SYSTEM_LOGS, "a+");
                fprintf(systemlogs, "Daemon is running\n");
                fclose(systemlogs);
        
                // Countdown to 23:30
                time(&now);
                double seconds_to_files_check = difftime(now, mktime(&check_uploads_time));

                systemlogs = fopen(SYSTEM_LOGS, "a+");
                fprintf(systemlogs, "%.f seconds until check for xml uploads\n", seconds_to_files_check);
                fclose(systemlogs);

                if(seconds_to_files_check == 0) {
                    check_file_uploads();
                    update_timer(&check_uploads_time); // Reset timer to 23:30
                }

                // Countdown to 1:00
                time(&now);
                double seconds_to_transfer = difftime(now, mktime(&backup_time));

                systemlogs = fopen(SYSTEM_LOGS, "a+");
                fprintf(systemlogs, "%.f seconds until backup\n", seconds_to_transfer);
                fclose(systemlogs);

                if(seconds_to_transfer == 0) {
                    lock_directories();
                    collect_reports();	  
                    backup_dashboard();
                    sleep(30);
                    unlock_directories();
                    generate_reports();
                    update_timer(&backup_time); // Reset timer to 1:00
                }	
            }
          }

	      closelog();
        return 0;
    }
}
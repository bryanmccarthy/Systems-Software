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
#include <time.h>
#include "daemon_task.h"
#include <signal.h>
#include <sys/inotify.h>

#define EVENT_SIZE    (sizeof (struct inotify_event))
#define EVENT_BUF_LEN (1024 * (EVENT_SIZE + 16))

int main() {
    FILE *systemlogs;
    FILE *xmllogs;
    int fd, wd, length, counter = 0; // File descriptor / Watch descriptor / Length of the event / Counter
    char buffer[EVENT_BUF_LEN]; // Buffer for inotify events

    // Create an inotify instance
    fd = inotify_init();

    if (fd < 0) {
      perror("inotify_init");
      exit(EXIT_FAILURE);
    }

    // Add a watch to the xml_upload_directory
    wd = inotify_add_watch(fd, "xml_upload_directory", IN_CREATE | IN_MODIFY | IN_DELETE);

    if (wd < 0) {
      perror("inotify_add_watch");
      exit(EXIT_FAILURE);
    }

    time_t now;
    struct tm backup_time;
    time(&now);  /* get current time; same as: now = time(NULL)  */
    backup_time = *localtime(&now);
    backup_time.tm_hour = 1; 
    backup_time.tm_min = 0; 
    backup_time.tm_sec = 0;

    // Create a child process      
    int pid = fork();
 
    if (pid > 0) {
        printf("Parent process: %d\n", getpid());
        sleep(3);
        exit(EXIT_SUCCESS);
    } else if (pid == 0) {
        // Step 1: Create the orphan process
      
        // Step 2: Elevate the orphan process to session leader, to loose controlling TTY
        // This command runs the process in a new session
        if (setsid() < 0) { exit(EXIT_FAILURE); }

        // We could fork here again , just to guarantee that the process is not a session leader
        int pid = fork();
        if (pid > 0) {
            printf("Parent process: %d\n", getpid());
            exit(EXIT_SUCCESS);
        } else {
            printf("Child process: %d, Childs parent: %d\n", getpid(), getppid());
            // Step 3: call umask() to set the file mode creation mask to 0
            umask(0);

            // Change dir to root
            if (chdir("/workspaces/Systems-Software/assignment-1") < 0) {
                exit(EXIT_FAILURE);
            }
            printf("Current working dir: %s\n", getcwd(NULL, 0));

            // Step 5: Close all open file descriptors
            // int maxfd = sysconf(_SC_OPEN_MAX);
            // for (int fd = 0; fd < maxfd; fd++) {
            //     close(fd);
            // }

            // Signal Handler
            if (signal(SIGUSR1, sig_handler) == SIG_ERR) {
                systemlogs = fopen("systemlogs.txt", "a+");
                fprintf(systemlogs, "ERROR: daemon.c : SIG_ERR RECEIVED");
                fclose(systemlogs);
            }
            
            struct tm check_uploads_time;
            time(&now);  /* get current time; same as: now = time(NULL)  */
            check_uploads_time = *localtime(&now);
            check_uploads_time.tm_hour = 23; 
            check_uploads_time.tm_min = 30; 
            check_uploads_time.tm_sec = 0;
	
            while(1) {
                sleep(1);

                counter = 0; // Reset counter
                length = read(fd, buffer, EVENT_BUF_LEN);

                if (length < 0) {
                    perror("read");
                    exit(EXIT_FAILURE);
                }
                
                while (counter < length) {
                    struct inotify_event *event = (struct inotify_event *) &buffer[counter];
                    if (event->len) {
                        xmllogs = fopen("xmllogs.txt", "a+"); // open xmllogs.txt

                        if (event->mask & IN_CREATE) {
                            if (event->mask & IN_ISDIR) {
                                fprintf(xmllogs, "The directory %s was created.\n", event->name);
                            } else {
                                fprintf(xmllogs, "The file %s was created.\n", event->name);
                            }
                        } else if (event->mask & IN_DELETE) {
                            if (event->mask & IN_ISDIR) {
                                fprintf(xmllogs, "The directory %s was deleted.\n", event->name);
                            } else {
                                fprintf(xmllogs, "The file %s was deleted.\n", event->name);
                            }
                        } else if (event->mask & IN_MODIFY) {
                            if (event->mask & IN_ISDIR) {
                                fprintf(xmllogs, "The directory %s was modified.\n", event->name);
                            } else {
                                fprintf(xmllogs, "The file %s was modified.\n", event->name);
                            }
                        }
                        fclose(xmllogs); // close xmllogs.txt
                    }
                    counter += EVENT_SIZE + event->len;
                }

                // kill(getpid(), SIGUSR1); // Testing
        
                //countdown to 23:30
                time(&now);
                double seconds_to_files_check = difftime(now,mktime(&check_uploads_time));

                systemlogs = fopen("systemlogs.txt", "a+");
                fprintf(systemlogs, "%.f seconds until check for xml uploads\n", seconds_to_files_check);
                fclose(systemlogs);

                if(seconds_to_files_check == 0) {
                    check_file_uploads();

                    //change to tommorow's day
                    update_timer(&check_uploads_time);
                }

                //countdown to 1:00
                time(&now);
                double seconds_to_transfer = difftime(now, mktime(&backup_time));

                systemlogs = fopen("systemlogs.txt", "a+");
                fprintf(systemlogs, "%.f seconds until backup\n", seconds_to_transfer);
                fclose(systemlogs);

                if(seconds_to_transfer == 0) {
                    lock_directories();
                    collect_reports();	  
                    backup_dashboard();
                    sleep(30);
                    unlock_directories();
                    generate_reports();
                    //after actions are finished, start counting to next day
                    update_timer(&backup_time);
                }	
            }
          }

	      closelog();
        return 0;
    }
}
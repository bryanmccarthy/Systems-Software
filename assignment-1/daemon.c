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

int main() {
    FILE *systemlogs;
    time_t now;

    struct tm backup_time;
    time(&now);
    backup_time = *localtime(&now);
    backup_time.tm_min += 1; // TODO: remove
    // backup_time.tm_hour = 1; 
    // backup_time.tm_min = 0; 
    // backup_time.tm_sec = 0;
    
    struct tm check_uploads_time;
    time(&now);
    check_uploads_time = *localtime(&now);
    check_uploads_time.tm_sec += 30; // TODO: remove
    // check_uploads_time.tm_hour = 23; 
    // check_uploads_time.tm_min = 30;
    // check_uploads_time.tm_sec = 0;

    // Create a child process      
    int pid = fork();
 
    if (pid > 0) {
        printf("Parent process: %d\n", getpid());
        sleep(3);
        exit(EXIT_SUCCESS);
    } else if (pid == 0) {
        printf("Forked: Child process: %d, Childs parent: %d\n", getpid(), getppid());
      
        // Elevate the orphan process to session leader to lose controlling TTY
        if (setsid() < 0) { exit(EXIT_FAILURE); }

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
            if (chdir("/workspaces/Systems-Software/assignment-1") < 0) {
                exit(EXIT_FAILURE);
            }

            // Close all open file descriptors
            int maxfd = sysconf(_SC_OPEN_MAX);
            for (int fd = 0; fd < maxfd; fd++) {
                close(fd);
            }

            // Signal Handler
            if (signal(SIGINT, sig_handler) == SIG_ERR) {
                systemlogs = fopen("systemlogs.txt", "a+");
                fprintf(systemlogs, "ERROR: daemon.c : SIG_ERR RECEIVED");
                fclose(systemlogs);
            }
	
            while(1) {
                sleep(5); // TODO: Change this to 1 second

                systemlogs = fopen("systemlogs.txt", "a+");
                fprintf(systemlogs, "Daemon is running\n");
                fclose(systemlogs);
        
                // Countdown to 23:30
                time(&now);
                double seconds_to_files_check = difftime(now, mktime(&check_uploads_time));

                systemlogs = fopen("systemlogs.txt", "a+");
                fprintf(systemlogs, "%.f seconds until check for xml uploads\n", seconds_to_files_check);
                fclose(systemlogs);

                if(seconds_to_files_check == 0) {
                    check_file_uploads();

                    // Reset timer to 23:30
                    update_timer(&check_uploads_time);
                }

                // Countdown to 1:00
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
                    // Reset timer to 1:00
                    update_timer(&backup_time);
                }	
            }
          }

	      closelog();
        return 0;
    }
}
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <syslog.h>
#include "daemon_task.h"
#include <sys/file.h>

void lock_directories() {
    FILE *systemlogs;

    systemlogs = fopen(SYSTEM_LOGS, "a+");
    fprintf(systemlogs, "Locking directories\n");
    fclose(systemlogs);
    
    // Lock the upload and reporting directories
    int upload_fd = open(UPLOAD_DIR, O_RDONLY);
    if (upload_fd == -1) {
        systemlogs = fopen(SYSTEM_LOGS, "a+");
        fprintf(systemlogs, "Error opening directory: %s\n", UPLOAD_DIR);
        fclose(systemlogs);
    }

    int fd2 = open(REPORTING_DIR, O_RDONLY);
    if (fd2 == -1) {
        systemlogs = fopen(SYSTEM_LOGS, "a+");
        fprintf(systemlogs, "Error opening directory: %s\n", REPORTING_DIR);
        fclose(systemlogs);
        close(upload_fd);
    }

    if (flock(upload_fd, LOCK_EX) == -1) {
        systemlogs = fopen(SYSTEM_LOGS, "a+");
        fprintf(systemlogs, "Failed to acquire lock on upload directory\n");
        fclose(systemlogs);
        close(upload_fd);
        close(fd2);
    }

    if (flock(fd2, LOCK_EX) == -1) {
        systemlogs = fopen(SYSTEM_LOGS, "a+");
        fprintf(systemlogs, "Failed to acquire lock on reporting directory\n");
        fclose(systemlogs);
        close(upload_fd);
        close(fd2);
    }

    systemlogs = fopen(SYSTEM_LOGS, "a+");
    fprintf(systemlogs, "Directories locked\n");
    fclose(systemlogs);

    sleep(20); // Sleep for 20 seconds
}

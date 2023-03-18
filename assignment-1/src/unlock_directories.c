#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <syslog.h>
#include "daemon_task.h"
#include <sys/file.h>

void unlock_directories() {
    FILE *systemlogs;

    systemlogs = fopen(SYSTEM_LOGS, "a+");
    fprintf(systemlogs, "Unlocking directories\n");
    fclose(systemlogs);

    // Unlock the upload and reporting directories
    int upload_fd = open(UPLOAD_DIR, O_RDONLY);
    if (upload_fd == -1) {
        systemlogs = fopen(SYSTEM_LOGS, "a+");
        fprintf(systemlogs, "Error opening directory: %s\n", UPLOAD_DIR);
        fclose(systemlogs);
    }

    int reporting_fd = open(REPORTING_DIR, O_RDONLY);
    if (reporting_fd == -1) {
        systemlogs = fopen(SYSTEM_LOGS, "a+");
        fprintf(systemlogs, "Error opening directory: %s\n", REPORTING_DIR);
        fclose(systemlogs);
        close(upload_fd);
    }

    if (flock(upload_fd, LOCK_UN) == -1) {
        systemlogs = fopen(SYSTEM_LOGS, "a+");
        fprintf(systemlogs, "Failed to acquire lock on upload directory\n");
        fclose(systemlogs);
        close(upload_fd);
        close(reporting_fd);
    }

    if (flock(reporting_fd, LOCK_UN) == -1) {
        systemlogs = fopen(SYSTEM_LOGS, "a+");
        fprintf(systemlogs, "Failed to acquire lock on reporting directory\n");
        fclose(systemlogs);
        close(upload_fd);
        close(reporting_fd);
    }

    systemlogs = fopen(SYSTEM_LOGS, "a+");
    fprintf(systemlogs, "Directories unlocked\n");
    fclose(systemlogs);
}

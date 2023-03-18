#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include "daemon_task.h"
#include <dirent.h>
#include <string.h>

void backup_dashboard(void) {
    FILE *systemlogs;

    systemlogs = fopen(SYSTEM_LOGS, "a+");
    fprintf(systemlogs, "\nBacking up dashboard\n");
    fclose(systemlogs);

    // char *backup_dir = BACKUP_DIR;
    // char *reporting_dir = REPORTING_DIR;

    char cp_command[256];
    snprintf(cp_command, sizeof(cp_command), "cp -a -u %s/* %s", REPORTING_DIR, BACKUP_DIR); // TODO: Fix this

    if (system(cp_command) == -1) {
        systemlogs = fopen(SYSTEM_LOGS, "a+");
        fprintf(systemlogs, "Error backing up dashboard\n");
        fclose(systemlogs);
    }

    systemlogs = fopen(SYSTEM_LOGS, "a+");
    fprintf(systemlogs, "Dashboard backed up\n");
    fclose(systemlogs);
}

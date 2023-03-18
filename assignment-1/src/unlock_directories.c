#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <syslog.h>
#include "daemon_task.h"
#include <sys/file.h>
#include <stdlib.h>

void unlock_directories() {
    FILE *systemlogs;
    char *unlock_permissions = "755";

    systemlogs = fopen(SYSTEM_LOGS, "a+");
    fprintf(systemlogs, "\nUnlocking directories\n");
    fclose(systemlogs);

    // Unlock the upload and reporting directories
    char unlock_upload[100];
    char unlock_reporting[100];

    sprintf(unlock_upload, "chmod %s %s", unlock_permissions, UPLOAD_DIR);
    sprintf(unlock_reporting, "chmod %s %s", unlock_permissions, REPORTING_DIR);

    system(unlock_upload);
    system(unlock_reporting);

    systemlogs = fopen(SYSTEM_LOGS, "a+");
    fprintf(systemlogs, "Directories unlocked\n\n");
    fclose(systemlogs);
}

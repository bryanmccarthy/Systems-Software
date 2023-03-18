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

void lock_directories() {
    FILE *systemlogs;
    char *lock_permissions = "777";

    systemlogs = fopen(SYSTEM_LOGS, "a+");
    fprintf(systemlogs, "\nLocking directories...\n");
    fclose(systemlogs);
    
    // Lock the upload and reporting directories
    char lock_upload[100];
    char lock_reporting[100];

    sprintf(lock_upload, "chmod %s %s", lock_permissions, UPLOAD_DIR);
    sprintf(lock_reporting, "chmod %s %s", lock_permissions, REPORTING_DIR);

    system(lock_upload);
    system(lock_reporting);

    systemlogs = fopen(SYSTEM_LOGS, "a+");
    fprintf(systemlogs, "Directories locked.\n");
    fclose(systemlogs);
}

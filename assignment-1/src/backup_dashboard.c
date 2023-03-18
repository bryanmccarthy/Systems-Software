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
    syslog(LOG_INFO, "Backing up dashboard");

    char cp_command[256];
    snprintf(cp_command, sizeof(cp_command), "cp -a -u %s/* %s", REPORTING_DIR, BACKUP_DIR); // TODO: Fix this

    if (system(cp_command) == -1) {
        syslog(LOG_ERR, "Error backing up dashboard\n");
    }
}

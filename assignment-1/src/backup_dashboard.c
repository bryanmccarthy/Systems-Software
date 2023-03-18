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

    DIR *dir;
    struct dirent *entry;

    // Open the backup directory
    dir = opendir(REPORTING_DIR);
    if (dir == NULL) {
        systemlogs = fopen(SYSTEM_LOGS, "a+");
        fprintf(systemlogs, "Error opening directory: %s\n", REPORTING_DIR);
        fclose(systemlogs);
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
      // Move entry to backup directory
      char *old_path = malloc(strlen(REPORTING_DIR) + strlen(entry->d_name) + 1);
      strcpy(old_path, REPORTING_DIR);
      strcat(old_path, entry->d_name);

      char *new_path = malloc(strlen(BACKUP_DIR) + strlen(entry->d_name) + 1);
      strcpy(new_path, BACKUP_DIR);
      strcat(new_path, entry->d_name);

      rename(old_path, new_path);

      free(old_path);
      free(new_path);
    }

    closedir(dir); // Close the upload directory

    systemlogs = fopen(SYSTEM_LOGS, "a+");
    fprintf(systemlogs, "Dashboard backed up\n\n");
    fclose(systemlogs);
}

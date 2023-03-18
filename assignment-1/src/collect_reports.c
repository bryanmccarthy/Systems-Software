#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <syslog.h>
#include <sys/types.h>
#include "daemon_task.h"
#include <dirent.h>
#include <string.h>

void collect_reports(void) {
    FILE *systemlogs;

    systemlogs = fopen(SYSTEM_LOGS, "a+");
    fprintf(systemlogs, "\nCollecting reports\n");
    fclose(systemlogs);

    DIR *dir;
    struct dirent *entry;

    // Open the upload directory
    dir = opendir(UPLOAD_DIR);
    if (dir == NULL) {
        systemlogs = fopen(SYSTEM_LOGS, "a+");
        fprintf(systemlogs, "Error opening directory: %s\n", UPLOAD_DIR);
        fclose(systemlogs);
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
      // Move entry to reporting directory
      char *old_path = malloc(strlen(UPLOAD_DIR) + strlen(entry->d_name) + 1);
      strcpy(old_path, UPLOAD_DIR);
      strcat(old_path, entry->d_name);

      char *new_path = malloc(strlen(REPORTING_DIR) + strlen(entry->d_name) + 1);
      strcpy(new_path, REPORTING_DIR);
      strcat(new_path, entry->d_name);

      rename(old_path, new_path);

      free(old_path);
      free(new_path);
    }

    closedir(dir); // Close the upload directory

    systemlogs = fopen(SYSTEM_LOGS, "a+");
    fprintf(systemlogs, "Done collecting reports\n\n");
    fclose(systemlogs);
}

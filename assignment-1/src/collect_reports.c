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
    syslog(LOG_INFO, "Collecting reports");

    DIR *dir;
    struct dirent *ent;

    // Open the upload directory
    dir = opendir(UPLOAD_DIR);
    if (dir == NULL) {
        syslog(LOG_ERR, "Error opening directory: %s", UPLOAD_DIR);
        return;
    }

    while ((ent = readdir(dir)) != NULL) {
      // Move file to reporting directory
      char *old_path = malloc(strlen(UPLOAD_DIR) + strlen(ent->d_name) + 1);
      strcpy(old_path, UPLOAD_DIR);
      strcat(old_path, ent->d_name);

      char *new_path = malloc(strlen(REPORTING_DIR) + strlen(ent->d_name) + 1);
      strcpy(new_path, REPORTING_DIR);
      strcat(new_path, ent->d_name);

      rename(old_path, new_path); // Move file

      free(old_path);
      free(new_path);
    }

    closedir(dir); // Close the upload directory
}

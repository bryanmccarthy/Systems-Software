#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <time.h>
#include "daemon_task.h"
#include <dirent.h>

void check_file_uploads() {
    FILE *systemlogs;
    char *xml_files[] = {"sales", "warehouse", "manufacturing", "distribution"}; // define in daemon_task.h
    int num_xml_files = 4;

    systemlogs = fopen(SYSTEM_LOGS, "a+");
    fprintf(systemlogs, "\nChecking for file uploads\n");
    fclose(systemlogs);

    DIR *dir;
    struct dirent *entry;
    int files_found[num_xml_files];

    // Initally set all files to not found
    for (int i = 0; i < num_xml_files; i++) {
        files_found[i] = 0;
    }

    // Open the upload directory
    dir = opendir(UPLOAD_DIR);
    if (dir == NULL) {
        systemlogs = fopen(SYSTEM_LOGS, "a+");
        fprintf(systemlogs, "Error opening directory: %s\n", UPLOAD_DIR);
        fclose(systemlogs);
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
      for (int i = 0; i < num_xml_files; i++) {
          if (strstr(entry->d_name, xml_files[i]) != NULL) {
              files_found[i] = 1; // Set file to found
          }
      }
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

    // Log files that were not found
    for (int i = 0; i < num_xml_files; i++) {
      if (!files_found[i]) {
        systemlogs = fopen(SYSTEM_LOGS, "a+");
        fprintf(systemlogs, "File %s not found\n", xml_files[i]);
        fclose(systemlogs);
      } 
    }

    systemlogs = fopen(SYSTEM_LOGS, "a+");
    fprintf(systemlogs, "Done Checking for file uploads\n\n");
    fclose(systemlogs);

    sleep(20);
}
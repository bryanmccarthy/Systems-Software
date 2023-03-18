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
    FILE *logfile;
    char *xml_files[] = {"sales", "warehouse", "manufacturing", "distribution"}; // define in daemon_task.h
    int num_xml_files = 4;

    systemlogs = fopen(SYSTEM_LOGS, "a+");
    fprintf(systemlogs, "\nChecking for file uploads...\n");
    fclose(systemlogs);

    DIR *dir;
    struct dirent *entry;
    int files_found[num_xml_files]; // 1 if file is found, 0 if not

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
      // Check if file is one of the xml files
      for (int i = 0; i < num_xml_files; i++) {
          if (strstr(entry->d_name, xml_files[i]) != NULL) {
              files_found[i] = 1; // Set file to found
          }
      }
    }

    closedir(dir); // Close the upload directory

    // Log files that were found or not found
    for (int i = 0; i < num_xml_files; i++) {
      if (files_found[i]) {
        // Log file found
        logfile = fopen(LOG_FILE, "a+");
        fprintf(logfile, "%s has been uploaded\n", xml_files[i]);
        fclose(logfile);
      } else {
        logfile = fopen(LOG_FILE, "a+");
        fprintf(logfile, "%s has not been uploaded\n", xml_files[i]);
        fclose(logfile);
      }
    }

    systemlogs = fopen(SYSTEM_LOGS, "a+");
    fprintf(systemlogs, "Done Checking for file uploads.\n\n");
    fclose(systemlogs);
}
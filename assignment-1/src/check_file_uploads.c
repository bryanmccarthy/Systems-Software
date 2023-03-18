#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <time.h>
#include "daemon_task.h"

void check_file_uploads(void) {
    FILE *systemlogs;

    systemlogs = fopen(SYSTEM_LOGS, "a+");
    fprintf(systemlogs, "\nChecking for file uploads\n");
    fclose(systemlogs);

    // DIR *dir;
    // struct dirent *ent;

    // if ((dir = opendir(UPLOAD_DIR)) != NULL) {
    //     while ((ent = readdir(dir)) != NULL) {
    //         if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0) {
    //             systemlogs = fopen(SYSTEM_LOGS, "a+");
    //             fprintf(systemlogs, "Found file: %s\n", ent->d_name);
    //             fclose(systemlogs);
    //         }
    //     }
    //     closedir(dir);
    // } else {
    //     systemlogs = fopen(SYSTEM_LOGS, "a+");
    //     fprintf(systemlogs, "ERROR: check_file_uploads.c : opendir() failed");
    //     fclose(systemlogs);
    // }

    systemlogs = fopen(SYSTEM_LOGS, "a+");
    fprintf(systemlogs, "Done Checking for file uploads\n\n");
    fclose(systemlogs);

    sleep(20);
}
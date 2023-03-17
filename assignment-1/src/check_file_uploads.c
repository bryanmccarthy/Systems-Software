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

    systemlogs = fopen("systemlogs.txt", "a+");
    fprintf(systemlogs, "\nChecking for file uploads\n");
    fclose(systemlogs);

    // ausearch
    // Search the audit log for events related to the watched directory
    char ausearch_cmd[256];
    snprintf(ausearch_cmd, sizeof(ausearch_cmd), "sudo ausearch -k directory-watch -f %s", UPLOAD_DIR);
    FILE *fp = popen(ausearch_cmd, "r");

    // Log the audit log to a text file
    FILE *log_fp = fopen(LOG_FILE, "a+");
    if (log_fp == NULL) {
        systemlogs = fopen("systemlogs.txt", "a+");
        fprintf(systemlogs, "ERROR: daemon.c : fopen() failed");
        fclose(systemlogs);
        exit(EXIT_FAILURE);
    }

    char line[1024];
    while (fgets(line, sizeof(line), fp) != NULL) {
        fprintf(log_fp, "%s", line);
    }

    fclose(log_fp);
    pclose(fp);

    systemlogs = fopen("systemlogs.txt", "a+");
    fprintf(systemlogs, "Done Checking for file uploads\n\n");
    fclose(systemlogs);
}
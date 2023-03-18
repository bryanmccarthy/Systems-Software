#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <time.h>
#include "daemon_task.h"

void check_file_uploads() {
    FILE *systemlogs;

    systemlogs = fopen(SYSTEM_LOGS, "a+");
    fprintf(systemlogs, "\nChecking for file uploads\n");
    fclose(systemlogs);

    

    systemlogs = fopen(SYSTEM_LOGS, "a+");
    fprintf(systemlogs, "Done Checking for file uploads\n\n");
    fclose(systemlogs);

    sleep(20);
}
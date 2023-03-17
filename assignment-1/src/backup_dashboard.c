#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

void backup_dashboard(void) {
    FILE *systemlogs;

    systemlogs = fopen("systemlogs.txt", "a+");
    fprintf(systemlogs, "Backing up dashboard\n");
    fclose(systemlogs);
}

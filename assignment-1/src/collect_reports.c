#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <syslog.h>
#include <sys/types.h>

void collect_reports(void) {
    FILE *systemlogs;

    systemlogs = fopen("systemlogs.txt", "a+");
    fprintf(systemlogs, "Collecting reports\n");
    fclose(systemlogs);
}

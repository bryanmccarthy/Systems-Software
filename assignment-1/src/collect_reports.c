#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <syslog.h>
#include <sys/types.h>
#include "daemon_task.h"

void collect_reports(void) {
    FILE *systemlogs;

    systemlogs = fopen(SYSTEM_LOGS, "a+");
    fprintf(systemlogs, "Collecting reports\n");
    fclose(systemlogs);
}

#include <unistd.h>
#include <syslog.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include "daemon_task.h"

void sig_handler(int sigNum) {
    FILE *systemlogs;

    if (sigNum == SIGINT) {
        systemlogs = fopen(SYSTEM_LOGS, "a+");
        fprintf(systemlogs, "RECEIVED SIGNAL INTERRUPT, INITIATING BACKUP AND TRANSFER\n");
        fclose(systemlogs);

        lock_directories();
        collect_reports();
        backup_dashboard();
        sleep(30);
        unlock_directories();	
    }
}
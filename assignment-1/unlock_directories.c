#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <syslog.h>

void unlock_directories() {
    FILE *systemlogs;

    systemlogs = fopen("systemlogs.txt", "a+");
    fprintf(systemlogs, "Unlocking directories\n");
    fclose(systemlogs);
}

#include <syslog.h>
#include <stdio.h>
#include <time.h>
#include "daemon_task.h"

void update_timer(struct tm *due_date) {
  FILE *systemlogs;

  systemlogs = fopen(SYSTEM_LOGS, "a+");
  fprintf(systemlogs, "Current day is %d; changing timer to tommorow's date\n", due_date -> tm_mday);
  fclose(systemlogs);

	due_date -> tm_mday += 1;
  mktime(due_date);	

  systemlogs = fopen(SYSTEM_LOGS, "a+");
	fprintf(systemlogs, "Timer updated to tommorow's date: %d\n", due_date -> tm_mday);
  fclose(systemlogs);
}

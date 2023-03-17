#include <syslog.h>
#include <stdio.h>
#include <time.h>

void update_timer(struct tm *due_date) {
  FILE *systemlogs;

  systemlogs = fopen("systemlogs.txt", "a+");
  fprintf(systemlogs, "Current day is %d; changing timer to tommorow's date\n", due_date -> tm_mday);
  fclose(systemlogs);

	due_date -> tm_mday += 1;
  mktime(due_date);	

  systemlogs = fopen("systemlogs.txt", "a+");
	fprintf(systemlogs, "Timer updated to tommorow's date: %d\n", due_date -> tm_mday);
  fclose(systemlogs);
}

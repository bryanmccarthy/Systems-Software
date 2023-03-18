#include <syslog.h>
#include <stdio.h>
#include <time.h>
#include "daemon_task.h"

void update_timer(struct tm *due_date) {
    syslog(LOG_INFO, "Updating timer to tommorow's date");

	  due_date -> tm_mday += 1; // Increment the day by 1
    mktime(due_date);	
}

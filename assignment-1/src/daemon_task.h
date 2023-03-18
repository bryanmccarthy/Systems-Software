#ifndef DAEMON_TASKS 
#define DAEMON_TASKS

#define UPLOAD_DIR "/workspaces/Systems-Software/assignment-1/directories/upload_directory/"
#define REPORTING_DIR "/workspaces/Systems-Software/assignment-1/directories/reporting_directory/"
#define BACKUP_DIR "/workspaces/Systems-Software/assignment-1/directories/backup_directory/"

#define LOG_FILE "/workspaces/Systems-Software/assignment-1/logs/log.txt"
#define SYSTEM_LOGS "/workspaces/Systems-Software/assignment-1/logs/systemlogs.txt"

#include <time.h>

void collect_reports(void);

void backup_dashboard(void);

void lock_directories(void);

void unlock_directories(void );

void generate_reports(void);

void check_file_uploads();

void sig_handler(int);

void update_timer(struct tm*);
#endif
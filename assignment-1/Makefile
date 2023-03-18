CC = gcc
CFLAGS = -Wall -g -pthread

daemon: daemon.o collect_reports.o backup_dashboard.o lock_directories.o unlock_directories.o generate_reports.o check_file_uploads.o sig_handler.o update_timer.o
		$(CC) $(CFLAGS) -o daemon daemon.o collect_reports.o backup_dashboard.o lock_directories.o unlock_directories.o generate_reports.o check_file_uploads.o sig_handler.o update_timer.o

daemon.o: src/daemon.c
		$(CC) $(CFLAGS) -c src/daemon.c

collect_reports.o: src/collect_reports.c
		$(CC) $(CFLAGS) -c src/collect_reports.c

backup_dashboard.o: src/backup_dashboard.c
		$(CC) $(CFLAGS) -c src/backup_dashboard.c

lock_directories.o: src/lock_directories.c
		$(CC) $(CFLAGS) -c src/lock_directories.c

unlock_directories.o: src/unlock_directories.c
		$(CC) $(CFLAGS) -c src/unlock_directories.c

generate_reports.o: src/generate_reports.c
		$(CC) $(CFLAGS) -c src/generate_reports.c

check_file_uploads.o: src/check_file_uploads.c
		$(CC) $(CFLAGS) -c src/check_file_uploads.c

sig_handler.o: src/sig_handler.c
		$(CC) $(CFLAGS) -c src/sig_handler.c

update_timer.o: src/update_timer.c
		$(CC) $(CFLAGS) -c src/update_timer.c

clean:
	rm -f *.o daemon
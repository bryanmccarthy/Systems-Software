CC = gcc
CFLAGS = -Wall -g

daemon: daemon.o collect_reports.o backup_dashboard.o lock_directories.o unlock_directories.o generate_reports.o check_file_uploads.o sig_handler.o update_timer.o
		$(CC) $(CFLAGS) -o daemon daemon.o collect_reports.o backup_dashboard.o lock_directories.o unlock_directories.o generate_reports.o check_file_uploads.o sig_handler.o update_timer.o

daemon.o: daemon.c
		$(CC) $(CFLAGS) -c daemon.c

collect_reports.o: collect_reports.c
		$(CC) $(CFLAGS) -c collect_reports.c

backup_dashboard.o: backup_dashboard.c
		$(CC) $(CFLAGS) -c backup_dashboard.c

lock_directories.o: lock_directories.c
		$(CC) $(CFLAGS) -c lock_directories.c

unlock_directories.o: unlock_directories.c
		$(CC) $(CFLAGS) -c unlock_directories.c

generate_reports.o: generate_reports.c
		$(CC) $(CFLAGS) -c generate_reports.c

check_file_uploads.o: check_file_uploads.c
		$(CC) $(CFLAGS) -c check_file_uploads.c

sig_handler.o: sig_handler.c
		$(CC) $(CFLAGS) -c sig_handler.c

update_timer.o: update_timer.c
		$(CC) $(CFLAGS) -c update_timer.c

clean:
	rm -f *.o daemon
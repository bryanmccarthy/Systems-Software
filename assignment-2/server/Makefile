CC = gcc
CFLAGS = -Wall -g -pthread

server: server.o
	$(CC) $(CFLAGS) -o server server.o

server.o: server.c
	$(CC) $(CFLAGS) -c server.c

clean:
	rm -f server server.o
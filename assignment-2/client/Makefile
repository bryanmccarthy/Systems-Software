CC = gcc
CFLAGS = -Wall -g -pthread

client: client.o
	$(CC) $(CFLAGS) -o client client.o

client.o: client.c
	$(CC) $(CFLAGS) -c client.c

clean:
	rm -f client client.o
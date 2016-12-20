CC=gcc
CFLAGS=-W -Wall
LDFLAGS=

client: client.o
	$(CC) -o client client.o $(LDFLAGS)

server: server.o
	$(CC) -o server server.o $(LDFLAGS)

server.o: server.c
	$(CC) -o server.o -c server.c $(CFLAGS)

client.o: client.c
	$(CC) -o client.o -c client.c $(CFLAGS)

clean:
	rm -rf *.o
	rm server
	rm client


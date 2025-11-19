all: server client

server: src/server.c
	$(CC) $(CFLAGS) -o server src/server.c

client: src/client.c
	$(CC) $(CFLAGS) -o client src/client.c

clean:
	rm -f server client

delete_all_tickets:
	# > inserisce contenuto vuoto nel file così da togliere tutti i tickets precedenti
	> tickets.txt

.PHONY: all clean delete_all_tickets
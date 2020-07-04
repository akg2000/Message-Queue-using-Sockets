all:server.c
	gcc -pthread server.c -o ser && ./ser

client:
	gcc -pthread client.c -o cli && ./cli

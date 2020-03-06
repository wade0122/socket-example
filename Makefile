
all:
	gcc -Wall -O0 -g -I. -lm tcp_server.c -o tcp_server.exe
	gcc -Wall -O0 -g -I. -lm tcp_client.c -o tcp_client.exe


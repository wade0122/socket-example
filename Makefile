
all:
	gcc -Wall -O0 -g -I. -lm tcp_server.c -o tcp_server.exe
	gcc -Wall -O0 -g -I. -lm tcp_client.c -o tcp_client.exe
	gcc -Wall -O0 -g -I. -lm udp_server.c -o udp_server.exe
	gcc -Wall -O0 -g -I. -lm udp_client.c -o udp_client.exe


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>

void PrintDateTime(void) {
    time_t nSeconds;
    struct tm * pTime;

    time(&nSeconds);
    pTime = localtime(&nSeconds);

    printf("-> %04d-%02d-%02d %02d:%02d:%02d ",
        pTime->tm_year + 1900, pTime->tm_mon + 1, pTime->tm_mday,
        pTime->tm_hour, pTime->tm_min, pTime->tm_sec);
        
    return ;
}

int main() {
	FILE *pFile = NULL;
    char buffer[2048];
    int len;
    int error;
    int clientsockfd;

    struct sockaddr_in serverAddr,clientAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));

    serverAddr.sin_family = PF_INET;
    serverAddr.sin_port = htons(23456);
    serverAddr.sin_addr.s_addr = inet_addr("169.254.3.147");// INADDR_ANY

    int serverSockfd = socket(AF_INET, SOCK_STREAM, 0);
    assert(serverSockfd != -1);

    error = bind(serverSockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    assert(error != -1);

    error = listen(serverSockfd,2);

    printf("wait client connecting...\n");

	pFile = fopen("./received-data-log.txt", "w");
    assert(pFile != NULL);

    static uint32_t total_len = 0;
    while(1){
        len = sizeof(clientAddr);
        clientsockfd = accept(serverSockfd, (struct sockaddr*)&clientAddr, &len);
        if(clientsockfd < 0){
            perror("client accept error\n");
            continue;
        }
        printf("client connected.\n");
        while(1)
        {
            memset((void*)buffer, 0x00, sizeof(buffer));
            len = recv(clientsockfd, buffer, sizeof(buffer), 0);
            if(len <= 0){
                continue;
             }
            total_len += len;
            PrintDateTime();
            //printf("cli %d total-rec %d:%s\n", clientsockfd, total_len, buffer);
            printf("cli %d total-rec %d\n", clientsockfd, total_len);
            send(clientsockfd, buffer, len, 0);
			if (pFile) {
				fwrite(buffer, 1, len, pFile);
				fflush(pFile);
			}
        }
        close(clientsockfd);
    }
	fclose(pFile);
    close(serverSockfd);
}



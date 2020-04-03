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

#define HOST_PORT 4002
#define HOST_IP "127.0.0.1"

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

    struct sockaddr_in serverAddr,clientAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));

    serverAddr.sin_family = PF_INET;
    serverAddr.sin_port = htons(HOST_PORT);
    serverAddr.sin_addr.s_addr = inet_addr(HOST_IP);//inet_addr("169.254.3.147");// htonl(INADDR_ANY)

    int serverSockfd = socket(PF_INET, SOCK_DGRAM, 0);
    assert(serverSockfd != -1);

    error = bind(serverSockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    assert(error != -1);


    printf("wait client data...\n");

	pFile = fopen("./received-data-log.txt", "w");
    assert(pFile != NULL);

    static int total_len = 0;
    int clientAddrLen = sizeof(clientAddr);
    while(1)
    {
        memset((void*)buffer, 0x00, sizeof(buffer));
        clientAddrLen = sizeof(clientAddr);
        len = recvfrom(serverSockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&clientAddr, &clientAddrLen);
        //len = recvfrom(serverSockfd, buffer, 10, 0, (struct sockaddr*)&clientAddr, &clientAddrLen);
        if(len <= 0){
            continue;
         }
        total_len += len;
        PrintDateTime();
        //printf("cli %d total-rec %d:%s\n", serverSockfd, total_len, buffer);
        printf("client %s total-rec %d\n", inet_ntoa(clientAddr.sin_addr), total_len);
        sendto(serverSockfd, buffer, len, 0, (struct sockaddr*)&clientAddr, clientAddrLen);
        if (pFile) {
            fwrite(buffer, 1, len, pFile);
            fflush(pFile);
        }
    }
	fclose(pFile);
    close(serverSockfd);
}



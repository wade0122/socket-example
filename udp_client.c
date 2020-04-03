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
#include <sys/time.h>

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

int file_send_test(void) {
	FILE *pFile = NULL;
	FILE *pFileR = NULL;
    char buffer[2048];
    int len;

    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));

    serverAddr.sin_family = PF_INET;
    serverAddr.sin_port = htons(HOST_PORT);
    serverAddr.sin_addr.s_addr = inet_addr(HOST_IP);//inet_addr("169.254.3.159");// INADDR_ANY

    int clientSockfd = socket(PF_INET, SOCK_DGRAM, 0);
    assert(clientSockfd != -1);

	pFile = fopen("./send-data", "r");
    assert(pFile != NULL);

	pFileR = fopen("./rec-data", "w");
    assert(pFileR != NULL);

    fseek(pFile, 0, SEEK_END);
	int f_len = ftell(pFile);

    fseek(pFile, 0, SEEK_SET);

    int send_len = 0;
    int receive_len = 0;
    printf("send...\n");
    int timeout = 0x00FF;
    struct timeval tv, last_tv;
    tv.tv_usec = 1;
    last_tv.tv_usec = 0;
    gettimeofday(&last_tv, NULL);
    while(timeout){
        if (f_len) {
            memset((void*)buffer, 0x00, sizeof(buffer));
            len = f_len>sizeof(buffer)?sizeof(buffer):f_len;
            len = fread(buffer, 1, len, pFile);
            if (len < 0) continue;
            sendto(clientSockfd, buffer, len, 0, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
            f_len -= len;
            send_len += len;
            timeout = 0x0FFF;
            usleep(1);
        }


        if (send_len % 1024 == 0) {
            printf("send length %d KB\n", send_len/1024);
            printf("rece length %f KB\n", receive_len/1024.0);
            gettimeofday(&tv, NULL);
            printf("time %ld us\n", tv.tv_sec*1000000 + tv.tv_usec - (last_tv.tv_sec*1000000 + last_tv.tv_usec));
            last_tv = tv;
        }

        memset((void*)buffer, 0x00, sizeof(buffer));
        struct sockaddr_in s_addr;
        int32_t s_addr_len;
        len = recvfrom(clientSockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&s_addr, &s_addr_len);
        if (len > 0) {
			fwrite(buffer, 1, len, pFileR);
            fflush(pFileR);
            receive_len += len;
            timeout = 0x0FFF;
        }
        
        timeout--;
    }
    printf("send all, %d Byte\n", send_len);
    printf("receive %d Byte\n", receive_len);
	fclose(pFile);
	fclose(pFileR);
    close(clientSockfd);
    return 0;
}

int loop_test(void) {
    char buffer[2048];
    int len;

    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));

    serverAddr.sin_family = PF_INET;
    serverAddr.sin_port = htons(HOST_PORT);
    serverAddr.sin_addr.s_addr = inet_addr(HOST_IP);//inet_addr("169.254.3.147");// INADDR_ANY

    int clientSockfd = socket(PF_INET, SOCK_DGRAM, 0);
    assert(clientSockfd != -1);

    printf("send...\n");
    while(1){
        memset((void*)buffer, 0x00, sizeof(buffer));
        len = sprintf(buffer, "0123456789");
        printf("send length %d:%s\n", len, buffer);
        sendto(clientSockfd, buffer, len, 0, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
        usleep(500000);
        memset((void*)buffer, 0x00, sizeof(buffer));
        struct sockaddr_in s_addr;
        int32_t s_addr_len;
        //len = recvfrom(clientSockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&s_addr, &s_addr_len);
        len = recvfrom(clientSockfd, buffer, 1, 0, (struct sockaddr*)&s_addr, &s_addr_len);
        printf("rec length %d:%s\n", len, buffer);
    }
    close(clientSockfd);
    return 0;
}

int main(){
   // file_send_test();
    loop_test();
    return 0;
}


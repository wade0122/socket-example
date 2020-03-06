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
    int error;

    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(4001);
    serverAddr.sin_addr.s_addr = inet_addr("169.254.3.159");// INADDR_ANY

    int clientSockfd = socket(AF_INET, SOCK_STREAM, 0);
    assert(clientSockfd != -1);

    error = connect(clientSockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    assert(error != -1);

    printf("connected server.\n");

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
            send(clientSockfd, buffer, len, 0);
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
        len = recv(clientSockfd, buffer, sizeof(buffer), 0);
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
    int error;

    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));

    serverAddr.sin_family = PF_INET;
    serverAddr.sin_port = htons(23456);
    serverAddr.sin_addr.s_addr = inet_addr("169.254.3.147");// INADDR_ANY

    int clientSockfd = socket(AF_INET, SOCK_STREAM, 0);
    assert(clientSockfd != -1);

    error = connect(clientSockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    assert(error != -1);

    printf("connected server.\n");

    printf("send...\n");
    while(1){
        memset((void*)buffer, 0x00, sizeof(buffer));
        len = sprintf(buffer, "0123456789");
        printf("send length %d:%s\n", len, buffer);
        send(clientSockfd, buffer, len, 0);
        usleep(500000);
        memset((void*)buffer, 0x00, sizeof(buffer));
        len = recv(clientSockfd, buffer, sizeof(buffer), 0);
        printf("rec length %d:%s\n", len, buffer);
    }
    close(clientSockfd);
    return 0;
}

int main(){
    file_send_test();
    //loop_test();
    return 0;
}


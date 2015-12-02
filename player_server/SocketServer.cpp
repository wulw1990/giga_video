#include "SocketServer.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define MAXLINE 4096

void init_server()
{
    int    listenfd, connfd;
    struct sockaddr_in     servaddr;
    struct sockaddr_in     client_addr;
    char    buff[4096];
    int     n;

    if ( (listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1 ) {
        printf("create socket error: %s(errno: %d)\n", strerror(errno), errno);
        exit(0);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(6666);

    if ( bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1) {
        printf("bind socket error: %s(errno: %d)\n", strerror(errno), errno);
        exit(0);
    }

    if ( listen(listenfd, 10) == -1) {
        printf("listen socket error: %s(errno: %d)\n", strerror(errno), errno);
        exit(0);
    }

    printf("======waiting for client's request======\n");
    while (1) {
        int len = sizeof(client_addr);
        if ( (connfd = accept(listenfd, (struct sockaddr*)&client_addr, (socklen_t * __restrict__)&len)) == -1) {
            printf("accept socket error: %s(errno: %d)", strerror(errno), errno);
            continue;
        }

        printf("%d.%d.%d.%d:%d\n",
               int(client_addr.sin_addr.s_addr & 0xFF),
               int((client_addr.sin_addr.s_addr & 0xFF00) >> 8),
               int((client_addr.sin_addr.s_addr & 0xFF0000) >> 16),
               int((client_addr.sin_addr.s_addr & 0xFF000000) >> 24),
               servaddr.sin_port);

        while(1){
                   n = recv(connfd, buff, MAXLINE, 0);
        buff[n] = '\0';
        if(n>0)
            printf("recv msg from client: %s\n", buff); 
        else
            break;
        }



        close(connfd);
    }

    close(listenfd);
}


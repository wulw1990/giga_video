#include "SocketServer.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include <vector>
#include <string>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;
#include "Protocol.hpp"


#define MAXLINE 4096

SocketServer* SocketServer::m_instance_p = NULL;
std::shared_ptr<SocketServer> SocketServer::m_instance_ptr;

void SocketServer::init(int port)
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
    servaddr.sin_port = htons(port);

    if ( bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1) {
        printf("bind socket error: %s(errno: %d)\n", strerror(errno), errno);
        exit(0);
    }

    if ( listen(listenfd, 10) == -1) {
        printf("listen socket error: %s(errno: %d)\n", strerror(errno), errno);
        exit(0);
    }

    printf("======waiting for client's request======\n");

    Protocol protocol;
    string cmd = "img";
    int length = 2;
    vector<unsigned char> data(2);
    data[0] = 0;
    data[1] = 1;

    vector<unsigned char> bu;
    protocol.encode(bu, cmd, data);




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


        VideoCapture capture("/home/wlw/Videos/video.mp4");
        assert(capture.isOpened());
        Mat frame;
        // while(capture.read(frame)){
        //     imshow("frame", frame);
        //     waitKey(33);
        // }

        while (1) {
            n = recv(connfd, buff, MAXLINE, 0);
            buff[n] = '\0';
            if (n > 0) {
                printf("recv msg from client: %s\n", buff);
                printf("len: %d\n", n);
            }
            else
                break;

            capture.read(frame);



            if ( send(connfd, "hh", 3, 0) < 0)
            {
                printf("send msg error: %s(errno: %d)\n", strerror(errno), errno);
                exit(0);
            }
        }
        close(connfd);
    }

    close(listenfd);
}


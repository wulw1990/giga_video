#include "SocketClient.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAXLINE 4096

int SocketClient::init(std::string server_ip)
{
	int    sockfd, n;
	char    recvline[4096], sendline[4096];
	struct sockaddr_in    servaddr;

	// if ( argc != 2) {
	// 	printf("usage: ./client <ipaddress>\n");
	// 	exit(0);
	// }

	if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("create socket error: %s(errno: %d)\n", strerror(errno), errno);
		exit(0);
	}

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(6666);
	if ( inet_pton(AF_INET, server_ip.c_str(), &servaddr.sin_addr) <= 0) {
		printf("inet_pton error for %s\n", server_ip.c_str());
		exit(0);
	}

	if ( connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
		printf("connect error: %s(errno: %d)\n", strerror(errno), errno);
		exit(0);
	}

	printf("send msg to server: \n");

	while (1) {
		fgets(sendline, 4096, stdin);
		// printf("len: %d\n", (int)strlen(sendline));
		if ( send(sockfd, sendline, strlen(sendline), 0) < 0)
		{
			printf("send msg error: %s(errno: %d)\n", strerror(errno), errno);
			exit(0);
		}
	}
	close(sockfd);
	exit(0);
}
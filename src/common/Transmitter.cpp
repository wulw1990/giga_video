#include "Transmitter.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

using namespace std;

int Transmitter::initSocketServer(int port) {
  int listenfd, connfd;
  struct sockaddr_in servaddr;
  struct sockaddr_in client_addr;
  char buff[4096];
  int n;

  if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    printf("create socket error: %s(errno: %d)\n", strerror(errno), errno);
    exit(0);
  }

  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(port);

  if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1) {
    printf("bind socket error: %s(errno: %d)\n", strerror(errno), errno);
    exit(0);
  }

  if (listen(listenfd, 10) == -1) {
    printf("listen socket error: %s(errno: %d)\n", strerror(errno), errno);
    exit(0);
  }

  return listenfd;
}
int Transmitter::initSocketClient(std::string ip, int port) {
  int sockfd, n;
  char recvline[4096], sendline[4096];
  struct sockaddr_in servaddr;

  // if ( argc != 2) {
  // 	printf("usage: ./client <ipaddress>\n");
  // 	exit(0);
  // }

  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    printf("create socket error: %s(errno: %d)\n", strerror(errno), errno);
    exit(0);
  }

  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(port);
  if (inet_pton(AF_INET, ip.c_str(), &servaddr.sin_addr) <= 0) {
    printf("inet_pton error for %s\n", ip.c_str());
    exit(0);
  }

  if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
    printf("connect error: %s(errno: %d)\n", strerror(errno), errno);
    exit(0);
  }

  printf("send msg to server: \n");
  return sockfd;
}
bool Transmitter::getClientId(int server_id, int &client_id) {
  struct sockaddr_in client_addr;
  int len = sizeof(client_addr);
  if ((client_id = accept(server_id, (struct sockaddr *)&client_addr,
                          (socklen_t * __restrict__) & len)) == -1) {
    printf("accept socket error: %s(errno: %d)", strerror(errno), errno);
    return false;
  }
  printf("%d.%d.%d.%d:%d\n", int(client_addr.sin_addr.s_addr & 0xFF),
         int((client_addr.sin_addr.s_addr & 0xFF00) >> 8),
         int((client_addr.sin_addr.s_addr & 0xFF0000) >> 16),
         int((client_addr.sin_addr.s_addr & 0xFF000000) >> 24),
         client_addr.sin_port);
  return true;
}
void Transmitter::closeSocket(int socket_id) { close(socket_id); }
bool Transmitter::sendData(int socket_id,
                           const std::vector<unsigned char> &data) {
  bool success(send(socket_id, &data[0], data.size(), 0) >= 0);
  if (!success) {
    printf("send msg error: %s(errno: %d)\n", strerror(errno), errno);
  }
  return success;
}
bool Transmitter::readData(int socket_id, std::vector<unsigned char> &data,
                           int len) {
  data.resize(len);
  int rec_len = 0;
  while (rec_len < len) {
    int n = recv(socket_id, &data[rec_len], len - rec_len, 0);
    rec_len += n;
    if (n <= 0) {
      return false;
    }
  }
  return true;
}

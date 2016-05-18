#ifndef TRANSMITTER_HPP_
#define TRANSMITTER_HPP_

#include <string>
#include <vector>

class Transmitter {
public:
  static int initSocketServer(int port);
  static int initSocketClient(std::string ip, int port);
  static bool getClientId(int server_id, int &client_id);
  static void closeSocket(int socket_id);

  static bool sendData(int socket_id, const std::vector<unsigned char> &data);
  static bool readData(int socket_id, std::vector<unsigned char> &data,
                       int len);
};

#endif
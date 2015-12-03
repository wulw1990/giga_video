#ifndef SOCKET_CLIENT_HPP_
#define SOCKET_CLIENT_HPP_

#include <string>

class SocketClient
{
public:
	SocketClient(){}
	~SocketClient(){}
	int init(std::string server_ip = "127.0.0.1");
};

#endif
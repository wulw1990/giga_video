#ifndef SOCKET_CLIENT_HPP_
#define SOCKET_CLIENT_HPP_

#include <string>
#include <memory>
class Protocol;
class Transmitter;

class SocketClient
{
public:
	SocketClient(std::string ip, int port);
	~SocketClient(){}
	void work();

private:
	std::shared_ptr<Protocol> m_protocol;
	std::shared_ptr<Transmitter> m_transmitter;
	int m_socket_id;
};

#endif
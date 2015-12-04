#ifndef SOCKET_SERVER_HPP_
#define SOCKET_SERVER_HPP_

#include <memory>
class Protocol;
class Transmitter;

class SocketServer
{
	public:
		SocketServer(int port) ;
		void work();

	private:

		std::shared_ptr<Protocol> m_protocol;
		std::shared_ptr<Transmitter> m_transmitter;

		int m_server_id;
};
void init_server();

#endif
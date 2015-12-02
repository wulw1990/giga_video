#ifndef SOCKET_SERVER_HPP_
#define SOCKET_SERVER_HPP_

#include <memory>

class SocketServer
{
public:
	static std::shared_ptr<SocketServer> getInstance(){
		if(m_instance_p==NULL){
			m_instance_p = new SocketServer();
			m_instance_ptr = std::shared_ptr<SocketServer>(m_instance_p);
		}
		return m_instance_ptr;
	} 


private:
	SocketServer(){}
	static SocketServer* m_instance_p;
	static std::shared_ptr<SocketServer> m_instance_ptr;
};
void init_server();

#endif
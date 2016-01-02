#ifndef SOCKET_CLIENT_HPP_
#define SOCKET_CLIENT_HPP_

#include <string>
#include <memory>
class Protocol;
class Transmitter;
class FrameProvider;

class Slave
{
	public:
		Slave(std::string path, std::string ip, int port, std::string name);
		~Slave() {}
		void work();

	private:
		std::shared_ptr<Protocol> m_protocol;
		std::shared_ptr<Transmitter> m_transmitter;
		int m_socket_id;

		std::string m_name;

		std::shared_ptr<FrameProvider> m_frame_provider;

		static void onMouse(int event, int x, int y, int, void* data);

};

#endif
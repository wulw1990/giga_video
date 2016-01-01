#ifndef SOCKET_CLIENT_HPP_
#define SOCKET_CLIENT_HPP_

#include <string>
#include <memory>
class Protocol;
class Transmitter;

class Slave
{
	public:
		Slave(std::string ip, int port);
		~Slave() {}
		void work();

	private:
		struct Info
		{
			int pre_x;
			int pre_y;
			int dx;
			int dy;
			int dz;
			bool update;
		};
		Info m_info;


		std::shared_ptr<Protocol> m_protocol;
		std::shared_ptr<Transmitter> m_transmitter;
		int m_socket_id;

		static void onMouse(int event, int x, int y, int, void* data);

};

#endif
#ifndef MASTER_HPP_
#define MASTER_HPP_

#include <memory>
#include <string>
class Protocol;
class Transmitter;
class FrameProvider;
class WindowController;

class Master
{
	public:
		Master(std::string path, int port) ;
		void work();

	private:

		std::shared_ptr<Protocol> m_protocol;
		std::shared_ptr<Transmitter> m_transmitter;

		int m_server_id;

		std::shared_ptr<FrameProvider> m_frame_provider;
		std::shared_ptr<WindowController> m_window_controller;

};
void init_server();

#endif
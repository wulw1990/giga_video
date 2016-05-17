#ifndef MASTER_HPP_
#define MASTER_HPP_

#include <memory>
#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
class Protocol;
class Transmitter;
class FrameProvider;
class WindowController;

class Master
{
	public:
		Master(std::string path, int port);
		void work();

	private:
		std::shared_ptr<Protocol> m_protocol;
		std::shared_ptr<Transmitter> m_transmitter;
		int m_server_id;
		std::vector<int> m_slave_id;


		struct Info
		{
			int pre_x;
			int pre_y;
			std::shared_ptr<FrameProvider> m_frame_provider;
			std::shared_ptr<WindowController> m_window_controller;
			bool update;
		};
		Info m_info;

		static void onMouse(int event, int x, int y, int, void* data);
		cv::Mat getFrame();
		cv::Mat readMatFromSocket(int socket_id, int color, std::string error);

};

#endif
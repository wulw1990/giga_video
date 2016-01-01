#ifndef _PLAYER_HPP_
#define _PLAYER_HPP_

#include <string>
#include <vector>
#include <memory>
#include <opencv2/opencv.hpp>

class FrameProvider;
class WindowController;

class Player
{
public:
	Player(std::string path, bool enable_video );
	~Player(){}
	
	void play();

	private:
		struct Info
		{
			int pre_x;
			int pre_y;
			int w;
			int h;
			std::shared_ptr<FrameProvider> m_frame_provider;
			std::shared_ptr<WindowController> m_window_controller;
			std::string win_title;
			cv::Mat frame;
			bool update;
		};
		Info m_info;

	static void onMouse(int event, int x, int y, int, void* data);
};

#endif
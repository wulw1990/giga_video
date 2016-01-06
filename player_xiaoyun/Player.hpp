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
		Player(std::string path, int n_frames, std::string output_video="" );
		~Player() {}

		void play();

	private:
		struct Info
		{
			int pre_x;
			int pre_y;
			std::shared_ptr<WindowController> m_window_controller;
			bool update;
		};
		Info m_info;

		std::vector<std::shared_ptr<FrameProvider> > m_frame_provider;
		int m_frame_id;

		cv::VideoWriter m_video_writer;

		static void onMouse(int event, int x, int y, int, void* data);
};

#endif
#ifndef IMAGE_PLAYER_HPP_
#define IMAGE_PLAYER_HPP_

#include <string>
#include <vector>
#include <memory>
#include <opencv2/opencv.hpp>

class SceneFrameProvider;

class ImagePlayer
{
public:
	ImagePlayer(std::string path, std::string info_file );
	~ImagePlayer(){}
	
	void play();

	private:
		struct Info
		{
			int pre_x;
			int pre_y;
			std::string m_window_title;
			int w;
			int h;
			double x;
			double y;
			double z;
			std::shared_ptr<SceneFrameProvider> m_frame_provider;
			cv::Mat frame;
		};
		Info m_info;

	static void onMouse(int event, int x, int y, int, void* data);
};

#endif
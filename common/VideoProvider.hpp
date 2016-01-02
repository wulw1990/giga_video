#ifndef VIDEO_PROVIDER_HPP_
#define VIDEO_PROVIDER_HPP_

#include <vector>
#include <string>
#include <fstream>
#include <memory>
#include <opencv2/opencv.hpp>

class CameraSetBase;

class VideoProvider{
	public:
		VideoProvider(std::string path);
		int getNumCamera();
		bool getRectOnScene(cv::Rect& rect, int index);
		bool getFrame(cv::Mat& frame, int index);

	private:
		std::string m_path;
		std::shared_ptr<CameraSetBase> m_camera_set;

		std::vector<cv::Mat> m_trans;
		std::vector<cv::Rect> m_rect;
};

#endif
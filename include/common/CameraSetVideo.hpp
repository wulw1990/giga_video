#ifndef CAMERA_SET_VITUAL_VIDEO_HPP_
#define CAMERA_SET_VITUAL_VIDEO_HPP_

#include "CameraSetBase.hpp"
#include <string>
#include <vector>

class CameraSetVideo: public CameraSetBase
{
	public:
		CameraSetVideo(const std::vector<std::string>& video_name);

		bool read(cv::Mat& frame, int index);
		int getNumCamera() {
			return (int)m_video_name.size();
		}
	private:
		std::vector<std::string> m_video_name;
		std::vector<cv::VideoCapture> m_capture;
};

#endif
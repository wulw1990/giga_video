#ifndef CAMERA_VITUAL_BHH_
#define CAMERA_VITUAL_BHH_

#include "CameraSetBase.hpp"
#include <string>
#include <vector>

class CameraSetVirtual: public CameraSetBase
{
	public:
		CameraSetVirtual(const std::vector<std::string>& video_name);

		bool read(cv::Mat& frame, int index);
		int getNumCamera() {
			return (int)m_video_name.size();
		}
	private:
		std::vector<std::string> m_video_name;
		std::vector<cv::VideoCapture> m_capture;
};

#endif
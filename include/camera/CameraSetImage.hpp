#ifndef CAMERA_SET_IMAGE_HPP_
#define CAMERA_SET_IMAGE_HPP_

#include "CameraSetBase.hpp"
#include <string>
#include <vector>

class CameraSetImage: public CameraSetBase
{
	public:
		CameraSetImage(const std::vector<std::string>& video_name);

		bool read(cv::Mat& frame, int index);
		int getNumCamera() {
			return (int)m_video_name.size();
		}
	private:
		std::vector<std::string> m_video_name;
		std::vector<std::vector<std::string>> m_frame_name;
		std::vector<int> m_frame_id;
};


#endif
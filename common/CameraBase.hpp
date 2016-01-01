#ifndef CAMERA_BASE_HPP_
#define CAMERA_BASE_HPP_

#include <opencv2/opencv.hpp>

class CameraBase {
	public:
		virtual int getNumCamera() = 0;
		virtual bool read(cv::Mat& frame, int index) = 0;
};

#endif
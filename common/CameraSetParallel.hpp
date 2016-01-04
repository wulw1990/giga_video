#ifndef CAMERA_SET_PARALLEL_HPP_
#define CAMERA_SET_PARALLEL_HPP_

#include "CameraSetBase.hpp"
#include <memory>
#include <vector>
#include <thread>

class CameraSetParallel: public CameraSetBase
{
public:
	CameraSetParallel();
	bool read(cv::Mat& frame, int index);
	int getNumCamera();
private:
	std::shared_ptr<CameraSetBase> m_camera_set;
	std::vector<cv::Mat> m_buffer;
	std::vector<std::thread> m_thread;
};

#endif
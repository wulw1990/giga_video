#ifndef CAMERA_SET_PARALLEL_HPP_
#define CAMERA_SET_PARALLEL_HPP_

#include "CameraSetBase.hpp"
#include <memory>
#include <vector>
#include <thread>
#include <mutex>

class CameraSetParallel: public CameraSetBase
{
public:
	CameraSetParallel();
	CameraSetParallel(std::shared_ptr<CameraSetBase> camera_set);
	bool read(cv::Mat& frame, int index);
	int getNumCamera();
private:
	std::shared_ptr<CameraSetBase> m_camera_set;
	std::vector<cv::Mat> m_buffer;
	std::vector<std::thread> m_thread;
	std::vector<std::mutex> m_locker;

	void setup();
};

#endif
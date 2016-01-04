#ifndef CAMERA_SET_RECORDER_HPP_
#define CAMERA_SET_RECORDER_HPP_

#include <string>
#include <memory>
#include <thread>
#include <vector>
class CameraSetBase;

class CameraSetRecorder
{
public:
	CameraSetRecorder(std::shared_ptr<CameraSetBase> camera_set);
	bool record(std::string path);

private:
	std::shared_ptr<CameraSetBase> m_camera_set; 
	std::vector<std::thread> m_thread;
};

#endif
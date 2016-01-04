#include "CameraSetParallel.hpp"

using namespace std;
using namespace cv;
#include "CameraSetFly2.hpp"

#include "Timer.hpp"

const int MS = 200;

static void internal_read(std::shared_ptr<CameraSetBase> camera_set, cv::Mat& frame, int index) {
	Timer timer;
	while (1) {
		timer.reset();
		camera_set->read(frame, index);
		int ms = timer.getTimeUs() / 1000;
		if (ms < MS) {
			std::this_thread::sleep_for(std::chrono::milliseconds(MS - ms));
		}
	}
}
CameraSetParallel::CameraSetParallel() {
	m_camera_set = make_shared<CameraSetFly2>();
	m_buffer.resize(m_camera_set->getNumCamera());
	m_thread.resize(m_camera_set->getNumCamera());
	for(int i=0; i<m_camera_set->getNumCamera(); ++i){
		Mat frame;
		m_camera_set->read(m_buffer[i], i);//init buffer
		assert(!m_buffer[i].empty());
		m_thread[i] = std::thread(internal_read, m_camera_set, std::ref(m_buffer[i]), i);
		// cout << m_buffer[i].size() << endl;
	}
}
bool CameraSetParallel::read(cv::Mat& frame, int index) {
	// return m_camera_set->read(frame, index);
	if(index<0 || index>=m_camera_set->getNumCamera()){
		return false;
	}
	if(m_buffer[index].empty()){
		return false;
	}
	frame = m_buffer[index];
	return true;
}
int CameraSetParallel::getNumCamera() {
	return m_camera_set->getNumCamera();
}
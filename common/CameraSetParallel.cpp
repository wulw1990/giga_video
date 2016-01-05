#include "CameraSetParallel.hpp"

using namespace std;
using namespace cv;
#include "CameraSetFly2.hpp"

#include "Timer.hpp"

const int RATE = 5;
const int MS = 1000/RATE;

// static void internal_read(std::shared_ptr<CameraSetBase> camera_set, cv::Mat& frame, int index) {
static void internal_read(std::shared_ptr<CameraSetBase> camera_set, cv::Mat& frame, std::mutex& locker, int index) {
	Timer timer;
	while (1) {
		timer.reset();
		Mat tmp;
		camera_set->read(tmp, index);
		locker.lock();
		tmp.copyTo(frame);
		locker.unlock();
		int ms = timer.getTimeUs() / 1000;
		if (ms < MS) {
			std::this_thread::sleep_for(std::chrono::milliseconds(MS - ms));
		}
	}
}
CameraSetParallel::CameraSetParallel() {
	m_camera_set = make_shared<CameraSetFly2>();
	setup();
}
CameraSetParallel::CameraSetParallel(std::shared_ptr<CameraSetBase> camera_set) {
	m_camera_set = camera_set;
	setup();
}
void CameraSetParallel::setup() {
	m_buffer.resize(m_camera_set->getNumCamera());
	m_thread.resize(m_camera_set->getNumCamera());
	// m_locker.resize(m_camera_set->getNumCamera());
	m_locker = vector<mutex>(m_camera_set->getNumCamera());
	for (int i = 0; i < m_camera_set->getNumCamera(); ++i) {
		Mat frame;
		m_camera_set->read(m_buffer[i], i);//init buffer
		assert(!m_buffer[i].empty());
		m_thread[i] = std::thread(internal_read, m_camera_set, std::ref(m_buffer[i]), std::ref(m_locker[i]), i);
	}
}
bool CameraSetParallel::read(cv::Mat& frame, int index) {
	// return m_camera_set->read(frame, index);
	if (index < 0 || index >= m_camera_set->getNumCamera()) {
		return false;
	}
	if (m_buffer[index].empty()) {
		return false;
	}
	m_locker[index].lock();
	frame = m_buffer[index].clone();
	m_locker[index].unlock();
	return true;
}
int CameraSetParallel::getNumCamera() {
	return m_camera_set->getNumCamera();
}
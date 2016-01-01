#include "CameraVirtual.hpp"

using namespace std;
using namespace cv;

CameraVirtual::CameraVirtual(const std::vector<std::string>& video_name){
	m_video_name = video_name;
	m_capture.resize(video_name.size());
}
bool CameraVirtual::read(cv::Mat& frame, int index){
	if(index>=(int)m_video_name.size()){
		return false;
	}
	if(!m_capture[index].isOpened()){
		m_capture[index].open(m_video_name[index]);
		if(!m_capture[index].isOpened()){
			return false;
		}
	}
	if(!m_capture[index].read(frame)){
		m_capture[index].release();
		m_capture[index].open(m_video_name[index]);
		if(!m_capture[index].isOpened()){
			return false;
		}
		if(!m_capture[index].read(frame)){
			return false;
		}
	}
	return true;
}

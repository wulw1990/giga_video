#include "CameraSetVideo.hpp"

using namespace std;
using namespace cv;

CameraSetVideo::CameraSetVideo(const std::vector<std::string> &video_name) {
  m_video_name = video_name;
  m_capture.resize(video_name.size());
  for (size_t i = 0; i < m_capture.size(); ++i) {
    m_capture[i].open(m_video_name[i]);
    if (!m_capture[i].isOpened()) {
      cerr << "CameraSetVideo: can not open video: " << m_video_name[i] << endl;
      exit(-1);
    }
  }
}
static bool readFrame(VideoCapture &capture, Mat &frame) {
  return capture.read(frame);
}
bool CameraSetVideo::read(cv::Mat &frame, int index) {
  if (index < 0 || index >= (int)m_video_name.size()) {
    return false;
  }
  if (!m_capture[index].isOpened()) {
    m_capture[index].open(m_video_name[index]);
    if (!m_capture[index].isOpened()) {
      return false;
    }
  }
  if (!readFrame(m_capture[index], frame)) {
    m_capture[index].release();
    m_capture[index].open(m_video_name[index]);
    if (!m_capture[index].isOpened()) {
      return false;
    }
    if (!readFrame(m_capture[index], frame)) {
      return false;
    }
  }
  return true;
}

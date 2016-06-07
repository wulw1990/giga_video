#include "WindowProviderLocal.hpp"

using namespace std;
using namespace cv;

#include "FrameProvider.hpp"
#include "Timer.hpp"

WindowProviderLocal::WindowProviderLocal(std::string path, int mode_video,
                                         cv::Size window_size) {
  m_size = window_size;
  m_position = Point3d(0.5, 0.5, 0);
  m_provider = make_shared<FrameProvider>(path, mode_video);
  m_has_frame = true;
  m_has_thumbnail = true;
}
void WindowProviderLocal::setWindowPosition(cv::Point3d position) {
  m_position = position;
}
bool WindowProviderLocal::hasFrame() {
  Timer timer;
  timer.reset();
  m_frame = m_provider->getFrameBackground(m_size, m_position);
  m_mask = Mat(m_frame.rows, m_frame.cols, CV_8UC1, 0);
  // cout << "Frame Time: " << timer.getTimeUs() / 1000 << " ms" << endl;

  bool need_update_foreground =
      m_provider->hasFrameForeground(m_size, m_position);
  if (need_update_foreground) {
    cv::Mat foregournd_frame;
    cv::Mat foregournd_mask;
    m_provider->getFrameForeground(m_size, m_position, foregournd_frame,
                                   foregournd_mask);
    foregournd_frame.copyTo(m_frame, foregournd_mask);
    m_mask = foregournd_mask;
  }
  m_has_frame = true;
  return true;
}
void WindowProviderLocal::getFrame(cv::Mat &frame, cv::Mat &mask) {
  frame = m_frame.clone();
  mask = m_mask.clone();
  m_has_frame = false;
}
bool WindowProviderLocal::hasThumbnail() {
  //
  return m_has_thumbnail;
}
void WindowProviderLocal::getThumbnail(std::vector<cv::Mat> &thumbnail,
                                       std::vector<cv::Point3d> &position) {
  m_provider->getThumbnail(thumbnail, position);
  m_has_thumbnail = false;
}
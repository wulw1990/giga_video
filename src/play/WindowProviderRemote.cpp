#include "WindowProviderRemote.hpp"

using namespace std;
using namespace cv;

#include "FrameProvider.hpp"
#include "Timer.hpp"

WindowProviderRemote::WindowProviderRemote(int socket_id,
                                           cv::Size window_size) {
  // send window_size

  // read thumnail and position

  // new thread
}
void WindowProviderRemote::setWindowPosition(cv::Point3d position) {
  // send position
}
bool WindowProviderRemote::hasFrame() {
  // m_has_frame updated by sub thread
  return m_has_frame;
}
void WindowProviderRemote::getFrame(cv::Mat &frame, cv::Mat &mask) {
  // frame mask update by sub thread
  frame = m_frame.clone();
  mask = m_mask.clone();
  m_has_frame = false;
}
void WindowProviderRemote::getThumbnail(std::vector<cv::Mat> &thumbnail,
                                        std::vector<cv::Point3d> &position) {
  thumbnail = m_thumbnail;
  position = m_position;
}
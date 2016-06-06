#ifndef WINDOW_PPOVIDER_REMOTE_HPP_
#define WINDOW_PPOVIDER_REMOTE_HPP_

#include <thread>
#include <mutex>
#include <memory>
#include "WindowProviderBase.hpp"

class WindowProviderRemote : public WindowProviderBase {
public:
  WindowProviderRemote(int socket_id, cv::Size window_size);
  void setWindowPosition(cv::Point3d position);
  bool hasFrame();
  void getFrame(cv::Mat &frame, cv::Mat &mask);
  bool hasThumbnail();
  void getThumbnail(std::vector<cv::Mat> &thumbnail,
                    std::vector<cv::Point3d> &position);

private:
  int m_socket_id;

  bool m_has_frame;
  cv::Mat m_frame;
  cv::Mat m_mask;
  std::thread m_thread_receive;
  std::mutex m_locker;
  
  bool m_has_thumnail;
  std::vector<cv::Mat> m_thumbnail;
  std::vector<cv::Point3d> m_position;
};

#endif
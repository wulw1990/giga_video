#ifndef WINDOW_PPOVIDER_REMOTE_HPP_
#define WINDOW_PPOVIDER_REMOTE_HPP_

#include <memory>
#include "WindowProviderBase.hpp"

class WindowProviderRemote : public WindowProviderBase {
public:
  WindowProviderRemote(int socket_id, cv::Size window_size);
  void setWindowPosition(cv::Point3d position);
  bool hasFrame();
  void getFrame(cv::Mat &frame, cv::Mat &mask);
  void getThumbnail(std::vector<cv::Mat> &thumbnail,
                    std::vector<cv::Point3d> &position);

private:
  int socket_id;

  bool m_has_frame;
  cv::Mat m_frame;
  cv::Mat m_mask;
  std::vector<cv::Mat> m_thumbnail;
  std::vector<cv::Point3d> m_position;
};

#endif
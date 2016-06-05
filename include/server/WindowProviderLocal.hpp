#ifndef WINDOW_PPOVIDER_LOCAL_HPP_
#define WINDOW_PPOVIDER_LOCAL_HPP_

#include <memory>
#include "WindowProviderBase.hpp"

class FrameProvider;

class WindowProviderLocal : public WindowProviderBase {
public:
  WindowProviderLocal(std::string path, int mode_video, cv::Size window_size);
  void setWindowPosition(cv::Point3d position);
  bool hasFrame();
  void getFrame(cv::Mat &frame, cv::Mat &mask);
  void getThumbnail(std::vector<cv::Mat> &thumbnail,
                    std::vector<cv::Point3d> &position);

private:
  cv::Size m_size;
  cv::Point3d m_position;
  std::shared_ptr<FrameProvider> m_provider;

  bool m_has_frame;
  cv::Mat m_frame;
  cv::Mat m_mask;
  std::vector<cv::Mat> m_thumbnail;
};

#endif
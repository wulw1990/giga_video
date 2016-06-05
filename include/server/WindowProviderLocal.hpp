#ifndef WINDOW_PPOVIDER_LOCAL_HPP_
#define WINDOW_PPOVIDER_LOCAL_HPP_

#include <memory>
#include "WindowProviderBase.hpp"

class FrameProvider;

class WindowProviderLocal : public WindowProviderBase {
public:
  WindowProviderLocal(std::string path, int mode_video, int w, int h);
  void setWindowPosition(double x, double y, double z);
  bool hasFrame();
  void getFrame(cv::Mat &frame);
  bool hasThumbnail();
  void getThumbnail(std::vector<cv::Mat> &thumbnail);

private:
  int m_window_width;
  int m_window_height;
  double m_window_x;
  double m_window_y;
  double m_window_z;
  std::shared_ptr<FrameProvider> m_provider;
};

#endif
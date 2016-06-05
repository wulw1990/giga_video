#ifndef WINDOW_PPOVIDER_BASE_HPP_
#define WINDOW_PPOVIDER_BASE_HPP_

#include <vector>
#include <string>
#include <opencv2/opencv.hpp>

class WindowProviderBase {
public:
  virtual void setWindowPosition(double x, double y, double z) = 0;
  virtual bool hasFrame() = 0;
  virtual void getFrame(cv::Mat &frame) = 0;
  virtual bool hasThumbnail() = 0;
  virtual void getThumbnail(std::vector<cv::Mat> &thumbnail) = 0;
};

#endif
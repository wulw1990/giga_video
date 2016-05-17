#ifndef WAITER_BASE_HPP_
#define WAITER_BASE_HPP_

#include <opencv2/opencv.hpp>
#include <vector>
#include <string>

class WaiterBase {
public:
  virtual void move(float dx, float dy) = 0;
  virtual void zoom(float dz) = 0;

  virtual bool hasFrame() = 0;
  virtual void getFrame(cv::Mat &frame) = 0;

  virtual bool hasThumbnail() = 0;
  virtual void getThumbnail(std::vector<cv::Mat> &thumbnail) = 0;

};

#endif
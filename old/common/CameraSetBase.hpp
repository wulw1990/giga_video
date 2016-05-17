#ifndef CAMERA_SET_BASE_HPP_
#define CAMERA_SET_BASE_HPP_

#include <opencv2/opencv.hpp>

class CameraSetBase {
public:
  virtual int getNumCamera() = 0;
  virtual bool read(cv::Mat &frame, int index) = 0;
};

#endif
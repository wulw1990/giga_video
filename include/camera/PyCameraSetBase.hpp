#ifndef PY_CAMERA_SET_BASE_HPP_
#define PY_CAMERA_SET_BASE_HPP_

#include <opencv2/opencv.hpp>

class PyCameraSetBase {
public:
  virtual int getNumCamera() = 0;
  virtual int getNumLayer() = 0;
  virtual bool read(cv::Mat &frame, int camera_id, int layer_id) = 0;
  virtual bool getCameraName(std::string &name, int camera_id) = 0;
};

#endif
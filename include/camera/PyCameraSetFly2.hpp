#ifndef PY_CAMERA_SET_FLY2_HPP_
#define PY_CAMERA_SET_FLY2_HPP_

#include "PyCameraSetBase.hpp"
#include <vector>
#include <string>
#include <memory>

class CameraSetFly2;

class PyCameraSetFly2 : public PyCameraSetBase {
public:
  PyCameraSetFly2(int n_layers);
  int getNumCamera();
  int getNumLayer();
  bool read(cv::Mat &frame, int camera_id, int layer_id);
  bool getCameraName(std::string &name, int camera_id);

private:
  int m_n_layers;
  std::shared_ptr<CameraSetFly2> m_camera_set;
};

#endif
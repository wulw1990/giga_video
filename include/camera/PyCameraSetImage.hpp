#ifndef PY_CAMERA_SET_IMAGE_HPP_
#define PY_CAMERA_SET_IMAGE_HPP_

#include "PyCameraSetBase.hpp"
#include <vector>
#include <string>

class PyCameraSetImage : public PyCameraSetBase {
public:
  PyCameraSetImage(std::string path);
  int getNumCameras();
  int getNumLayers();
  bool read(cv::Mat &frame, int camera_id, int layer_id);

private:
  std::string m_path;
  std::vector<std::string> m_camera_name;
  std::vector<std::string> m_layer_name;
  std::vector<std::vector<std::string>> m_camera_frame_name;
  std::vector<int> m_frame_id;
};

#endif
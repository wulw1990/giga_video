#ifndef PYRAMID_CAMERA_IMAGE_HPP_
#define PYRAMID_CAMERA_IMAGE_HPP_

#include "PyramidCamaraBase.hpp"

#include <vector>
#include <string>
#include <opencv2/opencv.hpp>

class PyramidCameraImage : public PyramidCameraBase {
public:
  PyramidCameraImage(std::string path);
  bool read(cv::Mat &frame, int layer_id);
  int getNumLayers();

private:
  std::string m_path;
  std::vector<std::string> m_layer_name;
  std::vector<std::vector<std::string>> m_frame_name;
  int m_frame_id;
};

#endif
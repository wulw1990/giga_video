#ifndef WINDOW_CONTROLLER_HPP_
#define WINDOW_CONTROLLER_HPP_

#include <vector>
#include <opencv2/opencv.hpp>

class WindowController {
public:
  WindowController(int n_layers, cv::Size top_layer_size, cv::Size winsize);
  void getPosition(cv::Point3d &position) {
    getXYZ(position.x, position.y, position.z);
  }
  void setPosition(cv::Point3d position) {
	  setXYZ(position.x, position.y, position.z);
  }
  void move(int dx, int dy);
  void zoom(double dz);

private:
  std::vector<cv::Size> m_layer_size;
  cv::Size m_winsize;

  double x;
  double y;
  double z;

  void getXYZ(double &x, double &y, double &z);
  void setXYZ(double x, double y, double z);
};

#endif
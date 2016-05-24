#include "PyramidAlignerAuto.hpp"

#include "FrameProvider.hpp"

using namespace std;
using namespace cv;

PyramidAlignerAuto::PyramidAlignerAuto(string path_scene) {
  m_frame_provider = make_shared<FrameProvider>(path_scene, 0);
}

void PyramidAlignerAuto::align(cv::Mat frame, cv::Mat &trans, cv::Rect &rect) {
  //
  trans = Mat(3, 3, CV_32FC1, Scalar(0.0f));
}

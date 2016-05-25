#ifndef PYRAMID_ALIGNER_AUTO_HPP_
#define PYRAMID_ALIGNER_AUTO_HPP_

#include <string>
#include <vector>
#include <memory>
#include <opencv2/opencv.hpp>
class FrameProvider;

class PyramidAlignerAuto {
public:
  PyramidAlignerAuto(std::string path_scene);
  void align(cv::Mat frame, cv::Mat &trans, cv::Rect &rect);
  static void alignZncc(cv::Mat frame, cv::Mat refer, cv::Mat &trans, cv::Rect &rect);
  static bool alignZncc(cv::Mat frame, cv::Mat refer, cv::Point& p);

private:
  std::shared_ptr<FrameProvider> m_frame_provider;
  
  static float zncc(const cv::Mat frame, const cv::Mat refer);
};

#endif
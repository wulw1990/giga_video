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
  
  //frame and refer must at same scale
  static void alignZncc(const cv::Mat frame, const cv::Mat refer, cv::Mat &trans, cv::Rect &rect);
  static bool alignZncc(const cv::Mat frame, const cv::Mat refer, cv::Point2f& p, float down_scale);

private:
  std::shared_ptr<FrameProvider> m_frame_provider;
  
  static float zncc(const cv::Mat frame, const cv::Mat refer);
};

#endif
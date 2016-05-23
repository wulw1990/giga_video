#ifndef PYRAMID_ALIGHER_HPP_
#define PYRAMID_ALIGHER_HPP_

#include <string>
#include <vector>
#include <memory>
#include <opencv2/opencv.hpp>
class FrameProvider;

class PyramidAligner {
public:
  PyramidAligner(std::string path_scene);
  void align(cv::Mat frame, cv::Mat &trans, cv::Rect &rect);

  bool alignRect(cv::Mat frame, cv::Mat refer, cv::Rect &rect);
  bool alignTransRect(cv::Mat frame, cv::Mat refer, cv::Mat &trans,
                      cv::Rect &rect);

private:
  std::shared_ptr<FrameProvider> m_frame_provider;

  static float getShowImage(cv::Mat src, cv::Mat &dst);

  struct MouseRectInfo {
    cv::string title;
    cv::Rect rect;
    cv::Mat image;
    bool move;
  };
  static void onMouseRect(int event, int x, int y, int, void *data);

  struct MousePointInfo {
    cv::string title;
    cv::Mat image;
    std::vector<cv::Point2f> point;
  };
  static void onMousePoint(int event, int x, int y, int, void *data);
  
  static bool existPoint(cv::Point2f p, std::vector<cv::Point2f>& v);
};

#endif
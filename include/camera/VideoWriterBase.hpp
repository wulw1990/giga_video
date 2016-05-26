#ifndef VIDEO_WRITER_BASE_HPP_
#define VIDEO_WRITER_BASE_HPP_

#include <opencv2/opencv.hpp>

class VideoWriterBase {
public:
  virtual void write(const cv::Mat frame) = 0;
  virtual void release() = 0;
};

#endif
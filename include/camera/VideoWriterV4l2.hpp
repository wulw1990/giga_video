#ifndef VIDEO_WRITER_V4L2_HPP_
#define VIDEO_WRITER_V4L2_HPP_

#include <opencv2/opencv.hpp>
#include <string>

class VideoWriterV4l2 {
public:
  VideoWriterV4l2(std::string device_path, cv::Size output_size);
  void writeFrame(const cv::Mat frame);
  void release();

private:
  size_t m_framesize;
  unsigned char *m_buffer;
  int m_fdwr;
  cv::Size m_output_size;
};

#endif
#ifndef VIDEO_PROVIDER_HPP_
#define VIDEO_PROVIDER_HPP_

#include <vector>
#include <string>
#include <fstream>
#include <memory>
#include <opencv2/opencv.hpp>

class PyCameraSetBase;

class VideoProvider {
public:
  VideoProvider(std::string path, bool online = false);
  int getNumCamera();
  bool getRect(cv::Rect &rect, int camera_id, int layer_id);
  bool getFrame(cv::Mat &frame, cv::Mat &mask, int camera_id, int layer_id);
  bool getThumbnail(cv::Mat& thumbnail, int camera_id);

private:
  int NUM_LAYERS;
  std::shared_ptr<PyCameraSetBase> m_camera_set;
  std::vector<std::vector<cv::Mat>> m_trans;
  std::vector<std::vector<cv::Rect>> m_rect;

  bool isValidLayer(int layer_id);
  bool isValidCamera(int camera_id);
};

#endif
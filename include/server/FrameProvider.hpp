#ifndef DATA_PROVIDER_HPP_
#define DATA_PROVIDER_HPP_

#include <memory>
#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
#include <map>

class VideoProvider;
class TileProvider;

class FrameProvider {
public:
  FrameProvider(std::string path, int video_mode);

  cv::Mat getFrameBackground(cv::Size size, cv::Point3d position) {
    return getFrameBackground(size.width, size.height, position.x, position.y,
                              position.z);
  }
  cv::Mat getFrameBackground(cv::Size size, cv::Point3i position) {
    return getFrameBackground(size.width, size.height, position.x, position.y,
                              position.z);
  }
  cv::Mat getFrameBackground(cv::Rect rect, int layer_id) {
    return getFrameBackground(rect.width, rect.height, rect.x, rect.y,
                              layer_id);
  }
  bool hasFrameForeground(cv::Size size, cv::Point3d position) {
    return hasFrameForeground(size.width, size.height, position.x, position.y,
                              position.z);
  }
  bool getFrameForeground(cv::Size size, cv::Point3d position, cv::Mat &frame,
                          cv::Mat &mask) {
    return getFrameForeground(size.width, size.height, position.x, position.y,
                              position.z, frame, mask);
  }
  void getThumbnail(std::vector<cv::Mat> &thumbnail,
                    std::vector<cv::Point3d> &position) {
    getThumbnail(thumbnail);
    std::vector<double> camera_x, camera_y, camera_z;
    getVideoPosition(camera_x, camera_y, camera_z);
    position.clear();
    for (size_t i = 0; i < camera_x.size(); ++i) {
      position.push_back(cv::Point3d(camera_x[i], camera_y[i], camera_z[i]));
    }
  }

  int getNumLayers();
  int getLayerWidth(int layer_id);
  int getLayerHeight(int layer_id);

private:
  cv::Mat getFrameBackground(int w, int h, double x, double y, double z);
  cv::Mat getFrameBackground(int w, int h, int x, int y, int z);

  bool hasFrameForeground(int w, int h, double x, double y, double z);
  bool hasFrameForeground(int w, int h, int x, int y, int z);
  bool getFrameForeground(int w, int h, double x, double y, double z,
                          cv::Mat &frame, cv::Mat &mask);
  bool getFrameForeground(int w, int h, int x, int y, int z, cv::Mat &frame,
                          cv::Mat &mask, std::vector<cv::Rect> &rect);

  bool getThumbnail(std::vector<cv::Mat> &thumbnail);
  bool getVideoPosition(std::vector<double> &x, std::vector<double> &y,
                        std::vector<double> &z);

private:
  std::shared_ptr<TileProvider> m_tile_provider;

  int m_video_mode;
  std::shared_ptr<VideoProvider> m_video_provider;

  void copyMatToMat(cv::Mat &src_mat, cv::Rect &src_rect, cv::Mat &dst_mat,
                    cv::Rect &dst_rect);
  void calculateSourceWindow(int w, int h, double x, double y, double z,
                             int source_layer_id, int *sw, int *sh, int *sx,
                             int *sy);
  int getNearestLayer(double z);
};

#endif
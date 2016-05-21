#ifndef DATA_PROVIDER_HPP_
#define DATA_PROVIDER_HPP_

#include <memory>
#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
#include <map>

class SceneData;
class MultiVideoData;
class VideoProvider;
class TileProvider;

class FrameProvider {
public:
  FrameProvider(std::string path, int video_mode);

  cv::Mat getFrameBackground(int w, int h, double x, double y, double z);
  cv::Mat getFrameBackground(int w, int h, int x, int y, int z);

  bool hasFrameForeground(int w, int h, double x, double y, double z);
  bool hasFrameForeground(int w, int h, int x, int y, int z);
  void getFrameForeground(int w, int h, double x, double y, double z,
                          cv::Mat &frame, cv::Mat &mask);
  void getFrameForeground(int w, int h, int x, int y, int z, cv::Mat &frame,
                          cv::Mat &mask);

  cv::Mat getFrame(int w, int h, int x, int y, int z) { return cv::Mat(); }

  int getNumLayers();
  int getLayerWidth(int layer_id);
  int getLayerHeight(int layer_id);

private:
  std::shared_ptr<TileProvider> m_tile_provider;

  int m_video_mode;
  // std::shared_ptr<MultiVideoData> m_multi_video_data;
  std::shared_ptr<VideoProvider> m_video_data;

  int frame_width;
  int frame_height;
  double x;
  double y;
  double z;

  void copyMatToMat(cv::Mat &src_mat, cv::Rect &src_rect, cv::Mat &dst_mat,
                    cv::Rect &dst_rect);

  void calculateSourceWindow(int w, int h, double x, double y, double z,
                             int source_layer_id, int *sw, int *sh, int *sx,
                             int *sy);

  int getNearestLayer(double z);
};

#endif
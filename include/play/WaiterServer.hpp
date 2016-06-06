#ifndef WAITER_SERVER_HPP_
#define WAITER_SERVER_HPP_

#include "WaiterBase.hpp"

#include <memory>
#include <thread>
#include <mutex>

class FrameProvider;
class WindowController;
class WindowProviderBase;

class WaiterServer : public WaiterBase {
public:
  WaiterServer(std::string path, cv::Size window_size, int video_mode,
               int port = -1);
  void move(float dx, float dy);
  void zoom(float dz);
  bool hasFrame();
  void getFrame(cv::Mat &frame);
  bool hasThumbnail();
  void getThumbnail(std::vector<cv::Mat> &thumbnail);
  void setThumbnailIndex(int index);

private:
  cv::Size m_window_size;
  std::vector<std::shared_ptr<WindowProviderBase>> m_window_provider;
  std::shared_ptr<WindowController> m_window_controller;
  void updateWindowPosition();

  bool m_has_frame;
  bool m_has_thumbnail;
  cv::Mat m_frame;
  void updateFrame();
  void getDesinationPosition(std::vector<cv::Point3d> &position);

  static void getLinearPath(cv::Point3d position_src, cv::Point3d position_dst,
                            int len, std::vector<cv::Point3d> &path);
  std::vector<cv::Point3d> m_auto_path;
  int m_auto_index;
  
  std::thread m_thread_listen;
};

#endif
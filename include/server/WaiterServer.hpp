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
  WaiterServer(std::string path, int window_width, int window_height,
               int video_mode);

  void move(float dx, float dy);
  void zoom(float dz);

  bool hasFrame();
  void getFrame(cv::Mat &frame);
  void getFrameMasked(cv::Mat &frame);

  void setThumbnailIndex(int index);
  bool hasThumbnail();
  void getThumbnail(std::vector<cv::Mat> &thumbnail);

private:
  int m_window_width;
  int m_window_height;
  std::shared_ptr<WindowProviderBase> m_window_local;
  std::shared_ptr<WindowController> m_window_controller;

  bool m_has_frame;
  bool m_has_thumbnail;

  cv::Mat m_frame;
  cv::Mat m_mask;
  std::vector<cv::Mat> m_thumbnail;

  static void getLinearPath(double sx, double sy, double sz, double dx,
                            double dy, double dz, int len,
                            std::vector<double> &vx, std::vector<double> &vy,
                            std::vector<double> &vz);
  std::vector<double> m_auto_x;
  std::vector<double> m_auto_y;
  std::vector<double> m_auto_z;
  int m_auto_index;

  void updateFrameBackground();
  void updateFrameForeground();
  bool m_need_update_foreground;

  std::mutex m_frame_locker;
  std::thread m_frame_foreground_thread;
};

#endif
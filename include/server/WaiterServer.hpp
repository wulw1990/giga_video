#ifndef WAITER_SERVER_HPP_
#define WAITER_SERVER_HPP_

#include "WaiterBase.hpp"

#include <memory>
#include <thread>
#include <mutex>

class FrameProvider;
class WindowController;

class WaiterServer : public WaiterBase {
public:
  WaiterServer(std::string path, int w, int h, int video_mode);
  void move(float dx, float dy);
  void zoom(float dz);

  bool hasFrame();
  void getFrame(cv::Mat &frame);

  bool hasThumbnail();
  void getThumbnail(std::vector<cv::Mat> &thumbnail);

private:
  std::shared_ptr<FrameProvider> m_frame_provider;
  std::shared_ptr<WindowController> m_window_controller;
  std::string m_path;
  int m_w;
  int m_h;
  
  bool m_has_frame;
  bool m_has_thumbnail;
  
  cv::Mat m_frame;
  std::vector<cv::Mat> m_thumbnail;
  
  void updateFrameBackground();
  void updateFrameForeground();
  bool m_need_update_foreground;
  
  std::mutex m_frame_locker;
  std::thread m_frame_foreground_thread;  
};

#endif
#ifndef PLAYER_AUTO_HPP_
#define PLAYER_AUTO_HPP_

#include <string>
#include <vector>
#include <memory>
#include <thread>
#include <mutex>
#include <opencv2/opencv.hpp>

class WaiterServer;

class PlayerAuto {
public:
  // video_mode:
  // 0: no video
  // 1: offline video
  // 2: online video
  PlayerAuto(std::string path, int video_mode, int port, std::string output_video = "");
  ~PlayerAuto() {}

  void play();

private:
  struct Info {
    int pre_x;
    int pre_y;
    std::shared_ptr<WaiterServer> m_waiter;
    cv::Mat frame;
    cv::Mat show;
    std::string win_title;
    int mouse_x;
    int mouse_y;
    cv::Scalar mouse_color;
    std::mutex show_locker;
    cv::Mat thumnail_show;
    std::vector<cv::Rect> thumnail_rect;
    int thumnail_index;
  };
  Info m_info;
  
  int m_window_width;
  int m_window_height;

  static void drawMouse(cv::Mat &show, int x, int y, cv::Scalar color);
  static void onMouse(int event, int x, int y, int, void *data);
  static int getThunbmailIndex(int x, int y, const std::vector<cv::Rect>& rect);

  std::thread m_record_thread;
  bool m_record_end;
  
  void updateThumbnail();
};

#endif
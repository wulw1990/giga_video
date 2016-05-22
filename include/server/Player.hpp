#ifndef _PLAYER_HPP_
#define _PLAYER_HPP_

#include <string>
#include <vector>
#include <memory>
#include <thread>
#include <mutex>
#include <opencv2/opencv.hpp>

class WaiterServer;

class Player {
public:
  // video_mode:
  // 0: no video
  // 1: offline video
  // 2: online video
  Player(std::string path, int video_mode, std::string output_video = "");
  ~Player() {}

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
  };
  Info m_info;

  static void onMouse(int event, int x, int y, int, void *data);

  std::thread m_record_thread;
  bool m_record_end;
};

#endif
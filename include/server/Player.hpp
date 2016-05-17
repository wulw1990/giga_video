#ifndef _PLAYER_HPP_
#define _PLAYER_HPP_

#include <string>
#include <vector>
#include <memory>
#include <opencv2/opencv.hpp>

class WaiterServer;

class Player {
public:
  Player(std::string path, bool enable_video, std::string output_video = "");
  ~Player() {}

  void play();

private:
  struct Info {
    int pre_x;
    int pre_y;
    std::shared_ptr<WaiterServer> m_waiter;
  };
  Info m_info;

  static void onMouse(int event, int x, int y, int, void *data);
  
  std::string m_output_video;
};

#endif
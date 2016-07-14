#ifndef PLAYER_SERVER_HPP_
#define PLAYER_SERVER_HPP_

#include <string>
#include <vector>
#include <memory>
#include <thread>
#include <mutex>
#include <opencv2/opencv.hpp>

class WaiterServer;

class PlayerServer {
public:
  // video_mode:
  // 0: no video
  // 1: offline video
  // 2: online video
  PlayerServer(std::string path, int video_mode, int port_slave, int port_client);
  ~PlayerServer() {}

  void play();

private:
  struct Info {
    int pre_x;
    int pre_y;
    std::shared_ptr<WaiterServer> m_waiter;
  };
  Info m_info;
  
  std::string m_window_title;
  int m_window_width;
  int m_window_height;

  static void onMouse(int event, int x, int y, int, void *data);
  
  int m_server_id;
};

#endif
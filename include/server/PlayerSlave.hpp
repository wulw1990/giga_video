#ifndef PLAYER_SLAVE_HPP_
#define PLAYER_SLAVE_HPP_

#include <string>
#include <vector>
#include <memory>
#include <thread>
#include <mutex>
#include <opencv2/opencv.hpp>

class WaiterServer;

class PlayerSlave {
public:
  // 0: no video
  // 1: offline video
  // 2: online video
  PlayerSlave(std::string path, int video_mode, std::string server_ip,
              int server_port);
  void play();

private:
  int m_window_width;
  int m_window_height;
  
  int m_client_id;
};

#endif
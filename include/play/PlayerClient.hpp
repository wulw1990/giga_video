#ifndef PLAYER_CLIENT_HPP_
#define PLAYER_CLIENT_HPP_

#include <string>
#include <vector>
#include <opencv2/opencv.hpp>

class PlayerClient {
public:
  PlayerClient(std::string server_ip, int server_port);
  void play();

private:
  int m_client_id;

  struct Info {
    int pre_x;
    int pre_y;
    int x;
    int y;
    int z;
  };
  Info m_info;

  std::string m_window_title;
  static void onMouse(int event, int x, int y, int, void *data);
};

#endif
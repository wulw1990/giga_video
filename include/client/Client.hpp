#ifndef CLIENT_HPP_
#define CLIENT_HPP_

#include <string>
#include <opencv2/opencv.hpp>

class Client {
public:
  Client(std::string server_ip, int server_socker_port);
  void run();

private:
  cv::VideoCapture m_capture;
  int m_server_socket_id;

  struct Info {
    int pre_x;
    int pre_y;
    cv::Mat frame;
    cv::Mat show;
    std::string win_title;
    int mouse_x;
    int mouse_y;
    cv::Scalar mouse_color;
    int dx;
    int dy;
    int dz;
  };
  Info m_info;

  static void drawMouse(cv::Mat &show, int x, int y, cv::Scalar color);
  static void onMouse(int event, int x, int y, int, void *data);
};

#endif
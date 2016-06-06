#ifndef PLAYER_SLAVE_HPP_
#define PLAYER_SLAVE_HPP_
#include <string>
#include <vector>
#include <memory>
#include <thread>
#include <mutex>
#include <opencv2/opencv.hpp>

class FrameProvider;

class PlayerSlave {
public:
  // 0: no video
  // 1: offline video
  // 2: online video
  PlayerSlave(std::string path, int video_mode, std::string server_ip,
              int server_port);
  void play();

private:
  cv::Size m_size;
  
  cv::Point3d m_position;
  std::thread m_thread_receive;
  std::mutex m_locker;

  int m_client_id;

  std::shared_ptr<FrameProvider> m_provider;
};
#endif
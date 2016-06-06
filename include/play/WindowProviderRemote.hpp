#ifndef WINDOW_PPOVIDER_REMOTE_HPP_
#define WINDOW_PPOVIDER_REMOTE_HPP_

#include <thread>
#include <mutex>
#include <memory>
#include <iostream>
#include "WindowProviderBase.hpp"

class WindowProviderRemote : public WindowProviderBase {
public:
  WindowProviderRemote(int socket_id, cv::Size window_size, std::string name);
  void setWindowPosition(cv::Point3d position);
  bool hasFrame();
  void getFrame(cv::Mat &frame, cv::Mat &mask);
  bool hasThumbnail();
  void getThumbnail(std::vector<cv::Mat> &thumbnail,
                    std::vector<cv::Point3d> &position);
  bool isDead() { return m_info.is_dead; }
  std::string getName() { return m_info.name; }

private:
  class Info {
  public:
    Info() {
      is_dead = false;
      has_thumnail = false;
      has_frame = false;
    }
    void die() {
      if (!is_dead) {
        std::cerr << "Dead Client: " << name << std::endl;
        thumbnail.clear();
        position.clear();
        has_thumnail = true; // update thumbnail
        frame = cv::Mat(window_size, CV_8UC3);
        mask = cv::Mat(window_size, CV_8UC1, cv::Scalar(0));
        is_dead = true;
      }
    }
    bool is_dead;
    int socket_id;
    bool has_frame;
    cv::Mat frame;
    cv::Mat mask;
    std::mutex locker;
    bool has_thumnail;
    std::vector<cv::Mat> thumbnail;
    std::vector<cv::Point3d> position;
    cv::Size window_size;
    std::string name;
  };
  Info m_info;
  std::thread m_thread_receive;

  static void work_receive(Info &info);
};

#endif
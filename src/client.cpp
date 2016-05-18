#include <iostream>
#include <cassert>
#include <algorithm>
#include <thread>
#include <memory>
using namespace std;

#include <opencv2/opencv.hpp>
using namespace cv;

#include "giga_video.h"
#include "server/Player.hpp"
#include "Transmitter.hpp"
#include "Protocol.hpp"

class Info {
public:
  int pre_x;
  int pre_y;
  int dx;
  int dy;
  int dz;
  int socket_id;
};

static void onMouse(int event, int x, int y, int, void *data);
static void work_receive(string window_name, int socket_id, Info &info,
                         bool &connect);

int main_internal_client(int argc, char **argv) {
  if (argc < 3) {
    cerr << "main_internal_client args error" << endl;
    return -1;
  }
  string server_ip(argv[1]);
  int server_port = atoi(argv[2]);

  int socket_id = Transmitter::initSocketClient(server_ip, server_port);

  Info info;
  info.pre_x = -1;
  info.pre_y = -1;
  info.dx = 0;
  info.dy = 0;
  info.dz = 0;
  info.socket_id = socket_id;

  // send window size
  vector<unsigned char> send_buf;
  Protocol::encodeXYZ(send_buf, 1700, 900, 0);
  if (!Transmitter::sendData(socket_id, send_buf)) {
    cout << "connect end" << endl;
    Transmitter::closeSocket(socket_id);
    return -1;
  }

  Mat tmp(100, 100, CV_8UC3);
  imshow("clinet", tmp);
  cv::setMouseCallback("clinet", onMouse, &info);
  waitKey(100);

  bool connect = true;
  thread thread_instance(work_receive, "clinet", socket_id, ref(info),
                         ref(connect));
  while (connect) {
    waitKey(100);
  }
  thread_instance.join();
  cout << "join" << endl;

  cout << "connect end" << endl;
  Transmitter::closeSocket(socket_id);
  return 0;
}

static void onMouse(int event, int x, int y, int, void *data) {
  if (event != EVENT_LBUTTONDOWN && event != EVENT_LBUTTONUP &&
      event != EVENT_LBUTTONDBLCLK && event != EVENT_RBUTTONDBLCLK)
    return;
  Info *info = (Info *)data;
  switch (event) {
  case EVENT_LBUTTONDOWN:
    info->pre_x = x;
    info->pre_y = y;
    break;
  case EVENT_LBUTTONUP:
    // cout << "x: " << info->pre_x << "-" << x << endl;
    // cout << "y: " << info->pre_y << "-" << y << endl;
    info->dx = info->pre_x - x;
    info->dy = info->pre_y - y;
    info->dz = 0;
    break;
  case EVENT_LBUTTONDBLCLK:
    info->dx = 0;
    info->dy = 0;
    info->dz = 1;
    break;
  case EVENT_RBUTTONDBLCLK:
    info->dx = 0;
    info->dy = 0;
    info->dz = -1;
    break;
  }
  if (event == EVENT_LBUTTONUP || event == EVENT_LBUTTONDBLCLK ||
      EVENT_RBUTTONDBLCLK) {
    if (info->dx != 0 || info->dy != 0 || info->dz != 0) {
      vector<unsigned char> send_buf;
      Protocol::encodeXYZ(send_buf, info->dx, info->dy, info->dz);
      if (!Transmitter::sendData(info->socket_id, send_buf)) {
        cout << "connect end" << endl;
        exit(-1);
      }
      info->dx = 0;
      info->dy = 0;
      info->dz = 0;
    }
  }
}

static void work_receive(string window_name, int socket_id, Info &info,
                         bool &connect) {
  while (1) {
    vector<unsigned char> recv_buf;
    if (!Transmitter::readData(socket_id, recv_buf, Protocol::getHeadLen())) {
      break;
    }
    string cmd;
    int data_len;
    Protocol::decodeHead(recv_buf, cmd, data_len);
    cout << "cmd: " << cmd << " data_len: " << data_len << endl;
    if (!Transmitter::readData(socket_id, recv_buf, data_len)) {
      break;
    }
    Mat raw(1, recv_buf.size(), CV_8UC1, &recv_buf[0]);
    Mat dec = imdecode(raw, 1);
    imshow(window_name, dec);
    cv::setMouseCallback(window_name, onMouse, &info);
    waitKey(1);
  }
  connect = false;
  cout << "connect end" << endl;
  Transmitter::closeSocket(socket_id);
}
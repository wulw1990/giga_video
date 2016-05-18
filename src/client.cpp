#include <iostream>
#include <cassert>
#include <algorithm>
using namespace std;

#include <opencv2/opencv.hpp>
using namespace cv;

#include "giga_video.h"
#include "server/Player.hpp"
#include "Transmitter.hpp"
#include "Protocol.hpp"

void onMouse(int event, int x, int y, int, void *data);

struct Info {
  int pre_x;
  int pre_y;
  int dx;
  int dy;
  int dz;
  bool update;
};

int main_internal_client(int argc, char **argv) {
  if (argc < 3) {
    cerr << "main_internal_client args error" << endl;
    return -1;
  }
  string server_ip(argv[1]);
  int server_port = atoi(argv[2]);

  Protocol protocol;
  Transmitter transmitter;
  int socket_id = transmitter.initSocketClient(server_ip, server_port);

  Info info;
  info.pre_x = -1;
  info.pre_y = -1;
  info.dx = 0;
  info.dy = 0;
  info.dz = 0;
  info.update = false;

  // send window size
  vector<unsigned char> send_buf;
  protocol.encodeXYZ(send_buf, 1700, 900, 0);
  if (!transmitter.sendData(socket_id, send_buf)) {
    cout << "connect end" << endl;
    transmitter.closeSocket(socket_id);
    return -1;
  }

  while (1) {
    vector<unsigned char> recv_buf;
    if (!transmitter.readData(socket_id, recv_buf, protocol.getHeadLen())) {
      break;
    }

    string cmd;
    int data_len;
    protocol.decodeHead(recv_buf, cmd, data_len);
    // cout << "cmd: " << cmd << " data_len: " << data_len << endl;
    if (!transmitter.readData(socket_id, recv_buf, data_len)) {
      break;
    }

    // for(int i=0; i<=255; ++i)
    //               cout << i << "\t" << (int)recv_buf[i] << endl;
    //           exit(-1);

    Mat raw(1, recv_buf.size(), CV_8UC1, &recv_buf[0]);
    Mat dec = imdecode(raw, 1);
    imshow("client", dec);
    cv::setMouseCallback("client", onMouse, &info);
    waitKey(30);

    int dx = 0;
    int dy = 0;
    int dz = 0;
    if (info.update) {
      dx = info.dx;
      dy = info.dy;
      dz = info.dz;
      // cout << dx << " " << dy << " " << dz << endl;
      info.update = false;
      info.dx = 0;
      info.dy = 0;
      info.dz = 0;
    }
    vector<unsigned char> send_buf;
    // vector<unsigned char> send_data(30);
    // m_protocol->encode(send_buf, "jpg", send_data);
    protocol.encodeXYZ(send_buf, dx, dy, dz);
    if (!transmitter.sendData(socket_id, send_buf)) {
      break;
    }
  }
  cout << "connect end" << endl;
  transmitter.closeSocket(socket_id);
  return 0;
}

void onMouse(int event, int x, int y, int, void *data) {
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
  info->update = true;
}

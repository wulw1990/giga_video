#include <iostream>
#include <cassert>
#include <algorithm>
#include <thread>
#include <mutex>
#include <memory>
using namespace std;

#include <opencv2/opencv.hpp>
using namespace cv;

#include "giga_video.h"
#include "server/Player.hpp"
#include "Transmitter.hpp"
#include "WaiterServer.hpp"
#include "Protocol.hpp"
#include "Timer.hpp"

const int MS = 30;
static void work_receive(shared_ptr<WaiterServer> waiter, int client_id,
                         bool &connect);

int main_internal_master(int argc, char **argv) {
  if (argc < 6) {
    cerr << "main_internal_master args error" << endl;
    return -1;
  }

  string path(argv[1]);
  int support_client = atoi(argv[2]);
  int video_online = atoi(argv[3]);
  int port = atoi(argv[4]);
  const int head_argc = 5;

  vector<string> slave_list;
  int n_slaves = argc - head_argc;
  slave_list.resize(n_slaves);
  for (int i = 0; i < n_slaves; ++i) {
    slave_list[i] = argv[head_argc + i];
    cout << "slave " << i << " : " << slave_list[i] << endl;
  }

  if (!support_client) {
    Player player(path, false);
    player.play();
    return 0;
  }

  int server_id = Transmitter::initSocketServer(port);
  while (1) {
    cout << "waiting for a client" << endl;
    int client_id;
    if (!Transmitter::getClientId(server_id, client_id)) {
      continue;
    }
    cout << "serve it" << endl;

    int window_width = 1000;
    int window_height = 500;
    {
      vector<unsigned char> recv_buf;
      if (!Transmitter::readData(client_id, recv_buf, Protocol::getHeadLen())) {
        cout << "connect end" << endl;
        break;
      }
      string cmd;
      int data_len;
      Protocol::decodeHead(recv_buf, cmd, data_len);
      // cout << "cmd: " << cmd << " data_len: " << data_len << endl;
      if (!Transmitter::readData(client_id, recv_buf, data_len)) {
        cout << "connect end" << endl;
        break;
      }
      int dx, dy, dz;
      Protocol::decodeDataXYZ(recv_buf, dx, dy, dz);
      window_width = dx;
      window_height = dy;
      cout << "window_width: " << window_width << endl;
      cout << "window_height: " << window_height << endl;
    }

    shared_ptr<WaiterServer> waiter =
        make_shared<WaiterServer>(path, window_width, window_height, 0);
    Timer timer;
    bool connect = true;
    thread thread_instance(work_receive, waiter, client_id, ref(connect));
    while (connect) {
      timer.reset();
      if (waiter->hasFrame()) {
        Mat frame;
        waiter->getFrame(frame);
        vector<unsigned char> jpg;
        imencode(".jpg", frame, jpg);
        vector<unsigned char> send_buf;
        Protocol::encode(send_buf, "img", jpg);
        if (!Transmitter::sendData(client_id, send_buf)) {
          break;
        }
        cout << "send ok" << endl;
      }
    } // serve it
    thread_instance.join();
    cout << "connect end" << endl;
    Transmitter::closeSocket(client_id);
    // destroyAllWindows();
  }
  Transmitter::closeSocket(server_id);
  return 0;
}

static void work_receive(shared_ptr<WaiterServer> waiter, int client_id,
                         bool &connect) {
  while (1) {
    vector<unsigned char> recv_buf;
    if (!Transmitter::readData(client_id, recv_buf, Protocol::getHeadLen())) {
      break;
    }
    string cmd;
    int data_len;
    Protocol::decodeHead(recv_buf, cmd, data_len);
    // cout << "cmd: " << cmd << " data_len: " << data_len << endl;
    if (!Transmitter::readData(client_id, recv_buf, data_len)) {
      break;
    }
    int dx, dy, dz;
    Protocol::decodeDataXYZ(recv_buf, dx, dy, dz);
    if (dx != 0 || dy != 0 || dz != 0) {
      cout << dx << " " << dy << " " << dz << endl;
    }
    cout << dx << " " << dy << " " << dz << endl;
    if (dx != 0 || dy != 0)
      waiter->move(dx, dy);
    if (dz != 0)
      waiter->zoom(dz);
  }
  connect = false;
}
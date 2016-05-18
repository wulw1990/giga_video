#include <iostream>
#include <cassert>
#include <algorithm>
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

int main_internal_master(int argc, char **argv) {
  if (argc < 4) {
    cerr << "main_internal_master args error" << endl;
    return -1;
  }

  string path(argv[1]);
  int support_client = atoi(argv[2]);
  int video_online = atoi(argv[3]);
  const int head_argc = 4;

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
  } else {
    Protocol protocol;
    Transmitter transmitter;
    const int port = 5000;
    int server_id = transmitter.initSocketServer(port);
    while (1) {
      cout << "waiting for a client" << endl;
      int client_id;
      if (!transmitter.getClientId(server_id, client_id)) {
        continue;
      }
      cout << "serve it" << endl;

      int window_width = 1000;
      int window_height = 500;
      {
        vector<unsigned char> recv_buf;
        if (!transmitter.readData(client_id, recv_buf, protocol.getHeadLen())) {
          cout << "connect end" << endl;
          break;
        }
        string cmd;
        int data_len;
        protocol.decodeHead(recv_buf, cmd, data_len);
        // cout << "cmd: " << cmd << " data_len: " << data_len << endl;
        if (!transmitter.readData(client_id, recv_buf, data_len)) {
          cout << "connect end" << endl;
          break;
        }
        int dx, dy, dz;
        protocol.decodeDataXYZ(recv_buf, dx, dy, dz);
        window_width = dx;
        window_height = dy;
        cout << "window_width: " << window_width << endl;
        cout << "window_height: " << window_height << endl;
      }

      WaiterServer waiter(path, window_width, window_height, 0);
      Timer timer;
      while (1) {
        timer.reset();
        Mat frame;
        waiter.getFrame(frame);
        // imshow("frame", frame);
        // waitKey(1);

        vector<unsigned char> jpg;
        imencode(".jpg", frame, jpg);
        // cout << jpg.size() << endl;

        // cout << "cmd: " << "img" << " data_len: " << jpg.size() << endl;
        // for(int i=0; i<=255; ++i){
        //     jpg[i] = i;
        // }
        // cout << (int)jpg[0] << endl;
        // cout << (int)jpg[255] << endl;

        vector<unsigned char> send_buf;
        protocol.encode(send_buf, "img", jpg);
        if (!transmitter.sendData(client_id, send_buf)) {
          break;
        }

        vector<unsigned char> recv_buf;
        if (!transmitter.readData(client_id, recv_buf,
                                     protocol.getHeadLen())) {
          break;
        }
        string cmd;
        int data_len;
        protocol.decodeHead(recv_buf, cmd, data_len);
        // cout << "cmd: " << cmd << " data_len: " << data_len << endl;
        if (!transmitter.readData(client_id, recv_buf, data_len)) {
          break;
        }
        int dx, dy, dz;
        protocol.decodeDataXYZ(recv_buf, dx, dy, dz);
        if (dx != 0 || dy != 0 || dz != 0) {
          cout << dx << " " << dy << " " << dz << endl;
          // for (size_t i = 0; i < recv_buf.size(); ++i) {
          //     cout << (int)recv_buf[i] << " ";
          // }
          // cout << endl;
        }
        // cout << dx << " " << dy << " " << dz << endl;
        waiter.move(dx, dy);
        waiter.zoom(dz);

        int ms = timer.getTimeUs() / 1000;
        cout << "ms: " << ms << endl;
        if (ms < MS) {
          waitKey(MS - ms);
        }
      }//serve it
      cout << "connect end" << endl;
      transmitter.closeSocket(client_id);
      // destroyAllWindows();
    }
    transmitter.closeSocket(server_id);
  }
return 0;
}

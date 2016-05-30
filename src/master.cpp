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
#include "DirDealer.hpp"
#include "PipeWriter.hpp"

const int MS = 30;
static void work_receive(shared_ptr<WaiterServer> waiter, int client_id);

int main_internal_master(int argc, char **argv) {
  if (argc < 4) {
    cerr << "main_internal_master args error" << endl;
    return -1;
  }

  string path(argv[1]);
  int mode_video = atoi(argv[2]);
  int port = atoi(argv[3]);

  int window_width = 1280;
  int window_height = 720;
  shared_ptr<WaiterServer> waiter =
      make_shared<WaiterServer>(path, window_width, window_height, mode_video);

  string name_pipe = "../pipe";
  DirDealer::rm_rf(name_pipe);
  PipeWriter pipe_writer(name_pipe);
  assert(pipe_writer.makePipe());
  thread sub_thread_ffserver = thread([name_pipe]() {
    string cmd = "cat " + name_pipe +
                 " | ffmpeg -v quiet -r 15 -f image2pipe -vcodec mjpeg "
                 "-i - -r 15 http://localhost:8090/feed1.ffm";
    cout << cmd << endl;
    DirDealer::systemInternal(cmd);
  });
  assert(pipe_writer.openPipe());

  thread sub_thread_connect = thread([waiter, port]() {
    int server_id = Transmitter::initSocketServer(port);
    while (1) {
      cout << endl << "waiting for a client" << endl;
      int client_id;
      string client_info;
      if (!Transmitter::getClientId(server_id, client_id, client_info)) {
        continue;
      }
      cout << endl << "serve it: " << client_info << endl;

      thread thread_instance(work_receive, waiter, client_id);
      thread_instance.join();
      Transmitter::closeSocket(client_id);
    }
    cout << endl << "connect end" << endl;
    Transmitter::closeSocket(server_id);
  });

  Mat frame;
  vector<unsigned char> jpg;
  Timer timer;
  while (1) {
    timer.reset();
    if (waiter->hasFrame()) {
      waiter->getFrame(frame);
      imshow("frame", frame);
      imencode(".jpg", frame, jpg);
    }
    assert(pipe_writer.writeSomething(jpg.data(), jpg.size()));

    int time = timer.getTimeUs() / 1000;
    int wait = max(1, 66 - time);
    char key = waitKey(wait);
    // cout << "time: " << time << " wait: " << wait << endl;
    cout << ".";
    cout.flush();
    if (key == 'q') {
      break;
    }
  }
  return 0;
}

static void work_receive(shared_ptr<WaiterServer> waiter, int client_id) {
  while (1) {
    vector<unsigned char> recv_buf;
    if (!Transmitter::readData(client_id, recv_buf, Protocol::getHeadLen())) {
      break;
    }
    string cmd;
    int data_len;
    Protocol::decodeHead(recv_buf, cmd, data_len);
    cout << endl << "cmd: " << cmd << " data_len: " << data_len << endl;
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
}
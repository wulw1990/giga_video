#include <iostream>
#include <cassert>
#include <algorithm>
#include <memory>
using namespace std;

#include <opencv2/opencv.hpp>
using namespace cv;

#include "giga_video.h"
#include "PlayerAuto.hpp"
#include "PlayerServer.hpp"
#include "PlayerClient.hpp"
#include "PlayerMannual.hpp"
#include "PlayerSlave.hpp"
#include "camera/CameraSetVideo.hpp"
#include "camera/CameraSetImage.hpp"
#include "camera/CameraSetFly2.hpp"
#include "camera/PyramidCameraImage.hpp"
#include "camera/PyCameraSetImage.hpp"
#include "camera/PyCameraSetFly2.hpp"
#include "Timer.hpp"

static int giga_image(int argc, char **argv);
static int giga_video(int argc, char **argv);
static int slave(int argc, char **argv);
static int server(int argc, char **argv);
static int client(int argc, char **argv);
static int camera_set(int argc, char **argv);
static int pyramid_camera(int argc, char **argv);
static int py_camera_set(int argc, char **argv);

int main_internal_demo(int argc, char **argv) {
  if (argc < 2) {
    cerr << "main_internal_demo args error" << endl;
    return -1;
  }
  string mode = string(argv[1]);
  argc--;
  argv++;
  transform(mode.begin(), mode.end(), mode.begin(), ::tolower);
  cout << "mode : " << mode << endl;
  if (mode == "giga_image")
    return giga_image(argc, argv);
  if (mode == "giga_video")
    return giga_video(argc, argv);
  if (mode == "slave")
    return slave(argc, argv);
  if (mode == "server")
    return server(argc, argv);
  if (mode == "client")
    return client(argc, argv);
  if (mode == "camera_set")
    return camera_set(argc, argv);
  if (mode == "pyramid_camera")
    return pyramid_camera(argc, argv);
  if (mode == "py_camera_set")
    return py_camera_set(argc, argv);
  else {
    cerr << "main_internal_demo mode error: " << mode << endl;
    return -1;
  }
  return 0;
}
static int giga_image(int argc, char **argv) {
  if (argc < 2) {
    cerr << "main_internal_demo giga_image args error." << endl;
    exit(-1);
  }
  string path(argv[1]);
  int mode_video = 0;
  PlayerAuto player(path, mode_video, -1);
  player.play();
  return 0;
}
static int giga_video(int argc, char **argv) {
  if (argc < 4) {
    cerr << "main_internal_demo giga_image args error." << endl;
    exit(-1);
  }
  string path(argv[1]);
  int mode_video = atoi(argv[2]);
  int port = atoi(argv[3]);
  PlayerAuto player(path, mode_video, port, "");
  player.play();
  return 0;
}
static int slave(int argc, char **argv) {
  if (argc < 5) {
    cerr << "main_internal_demo giga_image args error." << endl;
    exit(-1);
  }
  string path(argv[1]);
  int mode_video = atoi(argv[2]);
  string server_ip(argv[3]);
  int server_port = atoi(argv[4]);
  PlayerSlave player(path, mode_video, server_ip, server_port);
  player.play();
  return 0;
}
static int server(int argc, char **argv) {
  if (argc < 5) {
    cerr << "main_internal_demo server args error." << endl;
    exit(-1);
  }
  string path(argv[1]);
  int mode_video = atoi(argv[2]);
  int port_slave = atoi(argv[3]);
  int port_client = atoi(argv[4]);
  PlayerServer player(path, mode_video, port_slave, port_client);
  player.play();
  return 0;
}
static int client(int argc, char **argv) {
  if (argc < 3) {
    cerr << "main_internal_demo client args error." << endl;
    exit(-1);
  }
  string server_ip(argv[1]);
  int server_port = atoi(argv[2]);
  PlayerClient player(server_ip, server_port);
  player.play();
  return 0;
}
static int camera_set(int argc, char **argv) {
  shared_ptr<CameraSetBase> camera_set;
#if 0
  string path("/home/wuliwei/ramdisk/zijing16/video/");
  vector<string> video_name;
  string name = "video_0/";
  video_name.push_back(path + "MVI_6878/" + name);
  video_name.push_back(path + "MVI_6880/" + name);
  video_name.push_back(path + "MVI_6881/" + name);
  video_name.push_back(path + "MVI_6883/" + name);
  video_name.push_back(path + "MVI_6885/" + name);
  video_name.push_back(path + "MVI_7294/" + name);
  camera_set = make_shared<CameraSetImage>(video_name);
#else
  camera_set = make_shared<CameraSetFly2>();
#endif

  int n_cameras = camera_set->getNumCamera();
  cout << "n_cameras: " << n_cameras << endl;

  vector<Mat> frame(n_cameras);
  bool end = false;
  Timer timer;
  const int MS = 66;
  float scale = 4;
  while (!end) {
    timer.reset();
    for (int i = 0; i < n_cameras; ++i) {
      if (!camera_set->read(frame[i], i)) {
        end = true;
        break;
      }
      resize(frame[i], frame[i],
             Size(frame[i].cols / scale, frame[i].rows / scale));
    }
    cout << "Time=" << timer.getTimeUs() / 1000 << " ";
    for (int i = 0; i < n_cameras; ++i) {
      imshow("camera_" + to_string(i), frame[i]);
    }
    int wait = max(1, int(MS - timer.getTimeUs() / 1000));
    cout << "wait=" << wait << endl;
    waitKey(wait);
  }
  return 0;
}
static int pyramid_camera(int argc, char **argv) {
  // if (argc < 2) {
  //   cerr << "main_internal_demo pyramid_camera args error." << endl;
  //   exit(-1);
  // }

  PyramidCameraImage camera("/home/wuliwei/ramdisk/zijing16/video/MVI_6878/");
  const int MS = 66;
  while (1) {
    int layer_id = rand() % 5;
    Timer timer;
    timer.reset();
    Mat frame;
    camera.read(frame, layer_id);
    imshow("frame", frame);
    int time = timer.getTimeUs() / 1000;
    cout << "Time: " << time << endl;
    int wait = max(1, MS - time);
    waitKey(wait);
  }

  return 0;
}
static int py_camera_set(int argc, char **argv) {
  if (argc < 2) {
    cerr << "main_internal_demo pyramid_camera args error." << endl;
    exit(-1);
  }

  string path(argv[1]);
  string mode_video(argv[2]);
  string name_video(argv[3]);

  shared_ptr<PyCameraSetBase> camera_set;

  if (mode_video == "fly2") {
    camera_set = make_shared<PyCameraSetFly2>(7);
  } else if (mode_video == "image") {
    camera_set =
        make_shared<PyCameraSetImage>("/home/wuliwei/ramdisk/zijing16/video/");
  } else {
    cerr << "mode_video error" << endl;
    exit(-1);
  }

  const int MS = 66;
  int camera_id = atoi(name_video.c_str());
  cout << "n_cameras: " << camera_set->getNumCamera() << endl;
  while (1) {
    // int layer_id = rand() % 5;
    int layer_id = 3;
    Timer timer;
    timer.reset();
    Mat frame;
    assert(camera_set->read(frame, camera_id, layer_id));
    imshow("frame", frame);
    int time = timer.getTimeUs() / 1000;
    cout << "Time: " << time << endl;
    int wait = max(1, MS - time);
    waitKey(wait);
  }

  return 0;
}
#include <iostream>
#include <cassert>
#include <algorithm>
#include <memory>
using namespace std;

#include <opencv2/opencv.hpp>
using namespace cv;

#include "giga_video.h"
#include "server/Player.hpp"
#include "server/PlayerMannual.hpp"
#include "camera/CameraSetVideo.hpp"
#include "camera/CameraSetImage.hpp"
#include "camera/CameraSetFly2.hpp"
#include "camera/PyramidCameraImage.hpp"
#include "camera/PyCameraSetImage.hpp"
#include "camera/PyCameraSetFly2.hpp"
#include "Timer.hpp"

static int giga_image(int argc, char **argv);
static int giga_video(int argc, char **argv);
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
  // Player player(path, 0, path + "record.avi");
  Player player(path, 0);
  player.play();
  return 0;
}
static int giga_video(int argc, char **argv) {
  if (argc < 3) {
    cerr << "main_internal_demo giga_image args error." << endl;
    exit(-1);
  }
  string path(argv[1]);
  int mode_video = atoi(argv[2]);
  // Player player(path, 1, path + "record.avi");
  Player player(path, mode_video);
  // PlayerMannual player(path, mode_video);
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
  
  if(mode_video=="fly2"){
   camera_set = make_shared<PyCameraSetFly2>(7);
  }else if(mode_video=="image"){
   camera_set = make_shared<PyCameraSetImage>("/home/wuliwei/ramdisk/zijing16/video/");
  }else{
    cerr << "mode_video error" << endl;
    exit(-1);
  }
  
  const int MS = 66;
  int camera_id = atoi(name_video.c_str());
  while (1) {
    // int layer_id = rand() % 5;
    int layer_id = 3;
    Timer timer;
    timer.reset();
    Mat frame;
    camera_set->read(frame, camera_id, layer_id);
    imshow("frame", frame);
    int time = timer.getTimeUs() / 1000;
    cout << "Time: " << time << endl;
    int wait = max(1, MS - time);
    waitKey(wait);
  }

  return 0;
}
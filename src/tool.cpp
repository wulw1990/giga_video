#include <iostream>
#include <cassert>
#include <algorithm>
using namespace std;
#include <opencv2/opencv.hpp>
using namespace cv;

#include "giga_video.h"
#include "server/Player.hpp"
#include "DirDealer.hpp"

static int video2image(int argc, char **argv);

int main_internal_tool(int argc, char **argv) {
  if (argc < 2) {
    cerr << "main_internal_slave args error" << endl;
    return -1;
  }
  string mode = string(argv[1]);
  argc--;
  argv++;
  transform(mode.begin(), mode.end(), mode.begin(), ::tolower);
  cout << "mode : " << mode << endl;
  if (mode == "video2image")
    return video2image(argc, argv);
  else {
    cerr << "main_internal_slave mode error: " << mode << endl;
    return -1;
  }
  return 0;
}
static int video2image(int argc, char **argv) {
  if (argc < 3) {
    cerr << "main_internal_slave run args error." << endl;
    exit(-1);
  }
  string name_video(argv[1]);
  string path_image(argv[2]);

  cout << "name_video: " << name_video << endl;
  cout << "path_image: " << path_image << endl;

  DirDealer::mkdir_p(path_image);

  VideoCapture capture(name_video);
  assert(capture.isOpened());
  Size size_output(1920 / 2, 1080 / 2);

  Mat frame;
  int n_frames = 3000;
  for (int i = 0; i < n_frames; ++i) {
    cout << i << " / " << n_frames << endl;
    capture.read(frame);
    if (frame.empty()) {
      capture.release();
      capture.open(name_video);
      assert(capture.isOpened());
      capture.read(frame);
      assert(!frame.empty());
    }
    cv::resize(frame, frame, size_output);
    
    string name = to_string(i);
    name = string(8-name.length(), '0') + name + ".jpg";
    // cout << name << endl;
    imwrite(path_image + name, frame);
  }
  return 0;
}

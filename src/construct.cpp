#include <iostream>
#include <cassert>
#include <algorithm>
#include <sstream>
#include <iomanip>
using namespace std;

#include <opencv2/opencv.hpp>
using namespace cv;

#include "giga_video.h"
#include "GigaImageMeta.hpp"
#include "IO.hpp"
#include "DirDealer.h"


static int giga_image_meta(int argc, char **argv);
static int video_pyramid(int argc, char **argv);

int main_internal_construct(int argc, char **argv) {
  if (argc < 2) {
    cerr << "main_internal_construct args error" << endl;
    return -1;
  }
  string mode = string(argv[1]);
  argc--;
  argv++;
  transform(mode.begin(), mode.end(), mode.begin(), ::tolower);
  cout << "mode : " << mode << endl;
  if (mode == "giga_image_meta")
    return giga_image_meta(argc, argv);
  if (mode == "video_pyramid")
    return video_pyramid(argc, argv);
  else {
    cerr << "main_internal_construct mode error: " << mode << endl;
    return -1;
  }
  return 0;
}
static int giga_image_meta(int argc, char **argv) {
  if (argc < 3) {
    cerr << "main_internal_construct giga_image args error." << endl;
    exit(-1);
  }
  string path(argv[1]);
  string meta_file(argv[2]);
  GigaImageMeta meta;
  meta.generateMetaFile(path, meta_file);
  cout << "giga_image_meta" << endl;
  return 0;
}
static void saveVideoFrames(vector<Mat> &frame, string path) {
  DirDealer::mkdir_p(path);
  for(int i=0; i<frame.size(); ++i){
    string full_name;
    {
      stringstream ss;
      ss << setfill('0') << setw(6) << i;
      full_name = path + ss.str() + ".jpg";
      // cout << "full_name: " << full_name << endl;
    }
    imwrite(full_name, frame[i]);
  }
}
static void video_pyramid_one(string path) {
  cout << path << endl;
  Mat trans;
  Rect rect;
  {
    ifstream fin;
    assert(IO::openIStream(fin, path + "/info.txt", "VideoData load"));
    assert(IO::loadTransMat(fin, trans));
    assert(IO::loadRect(fin, rect));
    cout << trans << endl;
    cout << rect << endl;
  }

  VideoCapture capture(path + "video.avi");
  vector<Mat> frame;
  while (1) {
    Mat tmp;
    capture >> tmp;
    if (tmp.empty())
      break;
    frame.push_back(tmp.clone());
  }
  cout << frame.size() << endl;

  if (frame.empty()) {
    cout << "video empty" << endl;
    exit(-1);
  }

  for (int i = 4; i >= 0; --i) {
    const int FPS = 15;
    string name_video = path + "video_" + to_string(i) + "/";
    string name_info = path + "info_" + to_string(i) + ".txt";

    saveVideoFrames(frame, name_video);

    ofstream fout;
    assert(IO::openOStream(fout, name_info, "Write Info"));
    assert(IO::saveTransMat(fout, trans));
    assert(IO::saveRect(fout, rect));

    for (size_t j = 0; j < frame.size(); ++j) {
      resize(frame[j], frame[j], Size(frame[j].cols / 2, frame[j].rows / 2));
    }
    trans.at<float>(0, 2) /= 2;
    trans.at<float>(1, 2) /= 2;
    rect.x /= 2;
    rect.y /= 2;
    rect.width /= 2;
    rect.height /= 2;
  }
}
static int video_pyramid(int argc, char **argv) {
  if (argc < 2) {
    cerr << "main_internal_construct video_pyramid args error." << endl;
    exit(-1);
  }
  string path(argv[1]);
  ifstream fin;
  vector<string> list;
  assert(IO::openIStream(fin, path + "list.txt", "list load"));
  assert(IO::loadStringList(fin, list));
  cout << "list.size=" << list.size() << endl;

  for (size_t i = 0; i < list.size(); ++i) {
    cout << i << endl;
    video_pyramid_one(path + list[i] + "/");
  }

  return 0;
}

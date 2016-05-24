#include <iostream>
#include <cassert>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <memory>
using namespace std;

#include <opencv2/opencv.hpp>
using namespace cv;

#include "giga_video.h"
#include "GigaImageMeta.hpp"
#include "IO.hpp"
#include "DirDealer.hpp"
#include "CameraSetVideo.hpp"
#include "GigaAlignerBase.hpp"
#include "GigaAlignerAuto.hpp"
#include "GigaAlignerManual.hpp"
#include "PyramidAligner.hpp"
#include "PyramidAlignerAuto.hpp"

static int giga_image_meta(int argc, char **argv);
static int video_pyramid(int argc, char **argv);
static int cut_video(int argc, char **argv);
static int construct_camera_set_video(int argc, char **argv);
static int construct_camera_set_video_manual(int argc, char **argv);
static int construct_camera_set_video_auto(int argc, char **argv);

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
  if (mode == "cut_video")
    return cut_video(argc, argv);
  if (mode == "construct_camera_set_video")
    return construct_camera_set_video(argc, argv);
  if (mode == "construct_camera_set_video_manual")
    return construct_camera_set_video_manual(argc, argv);
  if (mode == "construct_camera_set_video_auto")
    return construct_camera_set_video_auto(argc, argv);
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
  for (int i = 0; i < (int)frame.size(); ++i) {
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
int cut_video(int argc, char **argv) {
  cout << "cut_video demo" << endl;
  assert(argc >= 3);
  string name_src(argv[1]);
  string name_dst(argv[2]);

  cout << "cutVideo: " << endl;
  cout << "name_src: " << name_src << endl;
  cout << "name_dst: " << name_dst << endl;

  assert(name_dst.substr(name_dst.length() - 3, 3) == "avi");

  Size output_size;

  VideoCapture capture(name_src.c_str());
  assert(capture.isOpened());
  vector<Mat> frame_vec;
  Mat frame;
  const int MAX_FRAMES = 1000;
  cout << "reading video" << endl;
  for (int i = 0; capture.read(frame) && i < MAX_FRAMES; ++i) {
    output_size = frame.size();
    resize(frame, frame, Size(frame.cols / 4, frame.rows / 4));
    frame_vec.push_back(frame);
  }
  capture.release();

  cout << "cut video..." << endl;
  int index_head = 0;
  int index_back = 0;
  cout << "index_head: " << index_head << " index_back: " << index_back << endl;
  for (int i = 0; i < (int)frame_vec.size(); ++i) {
    frame = frame_vec[i];
    const int PAD = frame.rows / 10;
    Mat show(frame.rows + PAD * 2, frame.cols, CV_8UC3, Scalar(255, 0, 0));
    frame.copyTo(show(Rect(0, 0, frame.cols, frame.rows)));
    rectangle(show,
              Rect(0, frame.rows, frame.cols * i / (int)frame_vec.size(), PAD),
              Scalar(0, 0, 255), -1);

    if (index_back > index_head) {
      int x = frame.cols * index_head / (int)frame_vec.size();
      int w = frame.cols * (index_back - index_head) / (int)frame_vec.size();
      rectangle(show, Rect(x, frame.rows + PAD, w, PAD), Scalar(0, 255, 0), -1);
    } else {
      int x = frame.cols * index_head / (int)frame_vec.size();
      rectangle(show, Rect(x, frame.rows + PAD, 2, PAD), Scalar(0, 255, 0), -1);
    }
    imshow("frame", show);
    char key = waitKey(0);
    if (key == 'b') {
      index_head = i;
      cout << "index_head: " << index_head << " index_back: " << index_back
           << endl;
    } else if (key == 'e') {
      index_back = i;
      cout << "index_head: " << index_head << " index_back: " << index_back
           << endl;
    } else if (key == 'p' && i > 0) {
      i -= 2;
    } else if (key == 'r') {
      i = -1;
    } else if (key == 'q') {
      break;
    }
  }
  destroyAllWindows();

  cout << "saving..." << endl;
  capture.open(name_src);
  assert(capture.isOpened());
  VideoWriter writer(name_dst, CV_FOURCC('M', 'J', 'P', 'G'), 15, output_size);
  assert(writer.isOpened());

  for (int i = 0; capture.read(frame), capture.read(frame); i += 2) {
    if (i >= index_head && i <= index_back) {
      writer << frame;
    }
  }
  capture.release();
  writer.release();
  return 0;
}

static int construct_camera_set_video(int argc, char **argv) {
  if (argc < 3) {
    cerr << "main_internal_construct construct_camera_set_video args error."
         << endl;
    exit(-1);
  }

  string path_scene(argv[1]);
  string path_video(argv[2]);
  string align_mode = "auto";
  shared_ptr<CameraSetBase> camera_set;
  shared_ptr<GigaAlignerBase> giga_aligner;

  if (align_mode == "auto") {
    giga_aligner = make_shared<GigaAlignerAuto>(path_scene);
  } else if (align_mode == "manual") {
    giga_aligner = make_shared<GigaAlignerManual>(path_scene);
  }

  vector<string> video_name;
  video_name.push_back(path_video + "/video.avi");
  camera_set = make_shared<CameraSetVideo>(video_name);

  int n_cameras = camera_set->getNumCamera();
  vector<Mat> frame(n_cameras);
  for (int i = 0; i < n_cameras; ++i) {
    assert(camera_set->read(frame[i], i));
  }

  for (int i = 0; i < 1; ++i) {
    cout << "align No." << i << " ..." << endl;
    Mat trans;
    Rect rect;
    if (giga_aligner->align(frame[i], trans, rect)) {
      std::ofstream fout;
      assert(IO::openOStream(fout, path_video + "/info.txt", "VideoData save"));
      assert(IO::saveTransMat(fout, trans));
      assert(IO::saveRect(fout, rect));
    }
  }

  return 0;
}
static int construct_camera_set_video_manual(int argc, char **argv) {
  if (argc < 3) {
    cerr << "main_internal_construct construct_camera_set_video args error."
         << endl;
    exit(-1);
  }

  string path_scene(argv[1]);
  string path_video(argv[2]);

  Mat frame;
  {
    vector<string> video_name;
    video_name.push_back(path_video + "/video.avi");
    CameraSetVideo camera_set(video_name);
    assert(camera_set.read(frame, 0));
  }

  PyramidAligner aligner(path_scene);
  Mat trans;
  Rect rect;
  aligner.align(frame, trans, rect);
  std::ofstream fout;
  assert(IO::openOStream(fout, path_video + "/info.txt", "VideoData save"));
  assert(IO::saveTransMat(fout, trans));
  assert(IO::saveRect(fout, rect));

  return 0;
}
static int construct_camera_set_video_auto(int argc, char **argv) {
  if (argc < 3) {
    cerr << "main_internal_construct construct_camera_set_video args error."
         << endl;
    exit(-1);
  }

  string path_scene(argv[1]);
  string path_video(argv[2]);

  Mat frame;
  {
    vector<string> video_name;
    video_name.push_back(path_video + "/video.avi");
    CameraSetVideo camera_set(video_name);
    assert(camera_set.read(frame, 0));
  }

  PyramidAlignerAuto aligner(path_scene);
  Mat trans;
  Rect rect;
  aligner.align(frame, trans, rect);
  std::ofstream fout;
  assert(IO::openOStream(fout, path_video + "/info.txt", "VideoData save"));
  assert(IO::saveTransMat(fout, trans));
  assert(IO::saveRect(fout, rect));

  return 0;
}
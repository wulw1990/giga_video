#include <iostream>
#include <cassert>
#include <algorithm>
#include <fstream>
#include <iterator>
#include <unistd.h>
using namespace std;

#include <opencv2/opencv.hpp>
using namespace cv;

#include "giga_video.h"
#include "construct/GigaImageMeta.hpp"
#include "DirDealer.hpp"
#include "Timer.hpp"
#include "PyramidAlignerAuto.hpp"
#include "VirtualCameraDevice.hpp"
#include "VideoWriterV4l2.hpp"
#include "PipeWriter.hpp"

static int giga_image_meta(int argc, char **argv);
static int read_image_tile(int argc, char **argv);
static int read_web_video(int argc, char **argv);
static int write_web_video(int argc, char **argv);
static int zncc(int argc, char **argv);
static int virtual_camera_device(int argc, char **argv);
static int video_writer_v4l2(int argc, char **argv);
static int frame_to_ffm(int argc, char **argv);
static int write_pipe(int argc, char **argv);
static int write_pipe_jpg(int argc, char **argv);

int main_internal_test(int argc, char **argv) {
  if (argc < 2) {
    cerr << "main_internal_test args error" << endl;
    return -1;
  }
  string mode = string(argv[1]);
  argc--;
  argv++;
  transform(mode.begin(), mode.end(), mode.begin(), ::tolower);
  cout << "mode : " << mode << endl;
  if (mode == "giga_image_meta")
    return giga_image_meta(argc, argv);
  if (mode == "read_image_tile")
    return read_image_tile(argc, argv);
  if (mode == "read_web_video")
    return read_web_video(argc, argv);
  if (mode == "write_web_video")
    return write_web_video(argc, argv);
  if (mode == "zncc")
    return zncc(argc, argv);
  if (mode == "virtual_camera_device")
    return virtual_camera_device(argc, argv);
  if (mode == "video_writer_v4l2")
    return video_writer_v4l2(argc, argv);
  if (mode == "frame_to_ffm")
    return frame_to_ffm(argc, argv);
  if (mode == "write_pipe")
    return write_pipe(argc, argv);
  if (mode == "write_pipe_jpg")
    return write_pipe_jpg(argc, argv);
  else {
    cerr << "main_internal_test mode error: " << mode << endl;
    return -1;
  }
  return 0;
}
static int giga_image_meta(int argc, char **argv) {
  if (argc < 3) {
    cerr << "main_internal_test giga_image args error." << endl;
    exit(-1);
  }
  string path(argv[1]);
  string meta_file(argv[2]);
  {
    GigaImageMeta meta;
    meta.generateMetaFile(path, meta_file);
  }
  {
    GigaImageMeta meta;
    meta.load(meta_file);
    int layers = meta.getLayers();
    cout << "layers: " << layers << endl;
    for (int layer_id = 0; layer_id < layers; ++layer_id) {
      int rows = meta.getRows(layer_id);
      int cols = meta.getCols(layer_id);
      cout << layer_id << ":" << endl;
      cout << "\trows:" << rows << "\tcols:" << cols << endl;
      for (int i = 0; i < 10; ++i) {
        int r = rand() % rows;
        int c = rand() % cols;
        cout << "\tr:" << r << "\tc:" << c << "\t"
             << meta.getTileName(layer_id, r, c) << endl;
      }
    }
  }
  cout << "giga_image_meta ok" << endl;
  return 0;
}
static int read_image_tile(int argc, char **argv) {
  if (argc < 2) {
    cerr << "main_internal_test read_image_tile args error." << endl;
    exit(-1);
  }

  string path(argv[1]);
  DirDealer dir_dealer;
  vector<string> list = dir_dealer.getFileList(path);
  cout << "list: " << list.size() << endl;

  std::vector<std::vector<char>> compress_data(list.size());
  std::vector<Mat> uncompress_data(list.size());

  Timer timer;
  timer.reset();
  for (size_t i = 0; i < list.size(); ++i) {
    std::ifstream file(path + list[i]);
    std::vector<char> data;

    file >> std::noskipws;
    std::copy(std::istream_iterator<char>(file), std::istream_iterator<char>(),
              std::back_inserter(data));

    compress_data[i] = data;
    // Mat mat = imdecode(Mat(data), 1);
    // imshow("mat", mat);
    // waitKey(0);
  }
  cout << "read time: " << timer.getTimeUs() / 1000 << endl;
  cout << "read time average: " << timer.getTimeUs() / 1000 / list.size()
       << endl;

  timer.reset();
  for (size_t i = 0; i < list.size(); ++i) {
    // uncompress_data[i] = imdecode(Mat(compress_data[i]), 1);
    Mat mat = imdecode(Mat(compress_data[i]), 1);
  }
  cout << "decode time: " << timer.getTimeUs() / 1000 << endl;
  cout << "decode time average: " << timer.getTimeUs() / 1000 / list.size()
       << endl;

  return 0;
}
static int read_web_video(int argc, char **argv) {
  if (argc < 2) {
    cerr << "main_internal_test read_image_tile args error." << endl;
    exit(-1);
  }

  string video_name(argv[1]);
  cout << "video_name: " << video_name << endl;

  VideoCapture capture(video_name);
  if (!capture.isOpened()) {
    cout << "camera can not open." << endl;
    exit(-1);
  }

  Mat frame;
  for (int i = 0; capture.read(frame); ++i) {
    capture >> frame;
    imshow("frame", frame);
    waitKey(30);
  }

  return 0;
}
static int zncc(int argc, char **argv) {
  if (argc < 4) {
    cerr << "main_internal_test read_image_tile args error." << endl;
    exit(-1);
  }

  string name_frame(argv[1]);
  string name_refer(argv[2]);
  int scale = atoi(argv[3]);

  Mat frame = imread(name_frame);
  Mat refer = imread(name_refer);
  cout << "frame: " << frame.size() << endl;
  cout << "refer: " << refer.size() << endl;
  cout << "scale: " << scale << endl;

  resize(frame, frame, Size(frame.cols / scale, frame.rows / scale));
  // imshow("frame", frame);
  // imshow("refer", refer);
  // waitKey(0);

  Mat trans;
  Rect rect;
  Timer timer;
  PyramidAlignerAuto::alignZncc(frame, refer, trans, rect);
  cout << "Time: " << timer.getTimeUs() / 1000 << " ms" << endl;
  cout << trans << endl;
  cout << rect << endl;

  Mat src_mask(frame.size(), CV_8UC1, Scalar(255));
  Mat dst_frame(rect.height, rect.width, CV_8UC3, Scalar(0, 0, 0));
  Mat dst_mask(rect.height, rect.width, CV_8UC1, Scalar(0));
  warpPerspective(frame, dst_frame, trans, dst_frame.size());
  warpPerspective(src_mask, dst_mask, trans, dst_mask.size());

  imshow("dst_frame", dst_frame);
  imshow("dst_mask", dst_mask);
  dst_frame.copyTo(refer(rect), dst_mask);
  imshow("refer", refer);
  waitKey(0);

  return 0;
}
static int write_web_video(int argc, char **argv) {
  cout << "write_web_video" << endl;

  string name_input = "../MVI_7305.avi";

  VideoCapture capture(name_input);
  assert(capture.isOpened());

  Size size_output(300, 200);
  cv::VideoWriter writer("http://localhost:8090/feed2.ffm",
                         CV_FOURCC('F', 'L', 'V', '1'), 25, size_output, true);
  assert(writer.isOpened());

  Mat frame;
  while (1) {
    capture.read(frame);
    if (frame.empty()) {
      capture.release();
      capture.open(name_input);
      assert(capture.isOpened());
      capture.read(frame);
      assert(!frame.empty());
    }
    cv::resize(frame, frame, size_output);
    imshow("frame", frame);
    waitKey(33);

    writer << frame;
  }

  return 0;
}
static int virtual_camera_device(int argc, char **argv) {
  VirtualCameraDevice device;
  device.test(argc, argv);

  return 0;
}
static int video_writer_v4l2(int argc, char **argv) {
  cout << "video_writer_v4l2" << endl;

  string name_input = "../MVI_7305.avi";
  string name_output = "/dev/video8";
  VideoCapture capture(name_input);
  assert(capture.isOpened());

  Size size_output(1920, 1080);
  VideoWriterV4l2 writer(name_output, size_output);

  Mat frame;
  while (1) {
    capture.read(frame);
    if (frame.empty()) {
      capture.release();
      capture.open(name_input);
      assert(capture.isOpened());
      capture.read(frame);
      assert(!frame.empty());
    }
    cv::resize(frame, frame, size_output);
    imshow("frame", frame);
    waitKey(33);

    writer.writeFrame(frame);
  }
  writer.release();
  return 0;
}
static int frame_to_ffm(int argc, char **argv) {
  cout << "frame_to_ffm" << endl;
  if (argc < 4) {
    cerr << "main_internal_test frame_to_ffm args error." << endl;
    exit(-1);
  }

  string input_video(argv[1]);
  string path_tmp(argv[2]);
  string ffm(argv[3]);

  DirDealer::mkdir_p(path_tmp);

  VideoCapture capture(input_video);
  assert(capture.isOpened());

  Size size_output(1920 / 2, 1080 / 2);

  Mat frame;
  Timer timer;
  while (1) {
    timer.reset();
    capture.read(frame);
    if (frame.empty()) {
      capture.release();
      capture.open(input_video);
      assert(capture.isOpened());
      capture.read(frame);
      assert(!frame.empty());
    }
    cv::resize(frame, frame, size_output);

    imwrite(path_tmp + "image0.jpg", frame);

    string cmd = "ffmpeg -f image2 -i " + path_tmp + "image%d.jpg " + ffm;
    cout << "cmd: " << cmd << endl;
    DirDealer::systemInternal(cmd.c_str());

    imshow("frame", frame);

    int time = timer.getTimeUs() / 1000;
    cout << "time: " << time << endl;
    int wait = max(1, 66 - time);
    char key = waitKey(wait);
    if (key == 'q') {
      break;
    }
  }

  return 0;
}

static int write_pipe(int argc, char **argv) {
  cout << "write_pipe" << endl;
  string name_pipe = "../pipe";
  DirDealer::rm_rf(name_pipe);
  Size size_output(1920 / 2, 1080 / 2);
  PipeWriter pipe_writer(name_pipe);
  assert(pipe_writer.makePipe());
  // DirDealer::systemInternal("cat " + name_pipe + "&");
  assert(pipe_writer.openPipe());
  Mat frame;
  for (int i = 0; i < 100; ++i) {
    cout << i << endl;
    sleep(1);
    string out = to_string(i) + '\n';
    assert(pipe_writer.writeSomething(out.c_str(), out.length()));
  }
  return 0;
}

static int write_pipe_jpg(int argc, char **argv) {
  // Usage:
  // cat ../pipe | ffmpeg -r 15 -f image2pipe -vcodec mjpeg -i - ../foo1.mp4
  // cat ../pipe | ffmpeg -r 15 -f image2pipe -vcodec mjpeg -i - -r 15
  // http://localhost:8090/feed1.ffm

  cout << "write_pipe_jpg" << endl;
  string name_pipe = "../pipe";
  string input_video = "../MVI_7305.avi";

  VideoCapture capture(input_video);
  assert(capture.isOpened());
  Size size_output(1920 / 2, 1080 / 2);

  DirDealer::rm_rf(name_pipe);
  PipeWriter pipe_writer(name_pipe);
  assert(pipe_writer.makePipe());
  // DirDealer::systemInternal("cat " + name_pipe + "&");
  assert(pipe_writer.openPipe());

  Mat frame;
  Timer timer;
  Rect rect(rand() % size_output.width, rand() % size_output.height, 200, 200);
  bool test_mask = false;
  for (int i = 0;; ++i) {
    cout << i << endl;
    timer.reset();
    capture.read(frame);
    if (frame.empty()) {
      capture.release();
      capture.open(input_video);
      assert(capture.isOpened());
      capture.read(frame);
      assert(!frame.empty());
    }
    cv::resize(frame, frame, size_output);

    if (test_mask) {
      frame.setTo(Scalar(255, 255, 255));
      if (i % 15 == 0) {
        rect = Rect(rand() % frame.cols, rand() % frame.rows, 200, 200);
      }
      rectangle(frame, rect, Scalar(0, 0, 0), -1);
    }

    imshow("frame", frame);

    vector<unsigned char> buf;
    cv::imencode(".jpg", frame, buf);

    // ofstream fout("../jj.jpg", ios::out | ios::binary);
    // fout.write((char*)buf.data(), buf.size());
    // break;

    // string out = to_string(i) + '\n';
    assert(pipe_writer.writeSomething(buf.data(), buf.size()));

    int time = timer.getTimeUs() / 1000;
    int wait = max(1, 66 - time);
    char key = waitKey(wait);
    cout << "time: " << time << " wait: " << wait << endl;
    if (key == 'q') {
      break;
    }
  }
  return 0;
}

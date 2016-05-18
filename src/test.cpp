#include <iostream>
#include <cassert>
#include <algorithm>
#include <fstream>
#include <iterator>
using namespace std;

#include <opencv2/opencv.hpp>
using namespace cv;

#include "giga_video.h"
#include "construct/GigaImageMeta.hpp"
#include "DirDealer.h"
#include "Timer.hpp"

static int giga_image_meta(int argc, char **argv);
static int read_image_tile(int argc, char **argv);

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

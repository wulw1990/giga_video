#include <iostream>
#include <cassert>
#include <algorithm>
using namespace std;

#include "giga_video.h"
#include "GigaImageMeta.hpp"

static int giga_image_meta(int argc, char **argv);

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
  else {
    cerr << "main_internal_construct mode error: " << mode << endl;
    return -1;
  }
  return 0;
}
static int giga_image_meta(int argc, char **argv) {
  if(argc < 3){
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

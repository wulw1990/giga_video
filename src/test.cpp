#include <iostream>
#include <cassert>
#include <algorithm>
using namespace std;

#include "giga_video.h"
#include "construct/GigaImageMeta.hpp"

static int giga_image_meta(int argc, char **argv);

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
    int layers= meta.getLayers();
    cout << "layers: " << layers << endl;
    for(int layer_id=0; layer_id<layers; ++layer_id){
        int rows = meta.getRows(layer_id);
        int cols = meta.getCols(layer_id);
        cout << layer_id << ":" << endl;
        cout << "\trows:" << rows << "\tcols:" << cols << endl;
        for(int i=0; i<10; ++i){
            int r = rand()%rows;
            int c = rand()%cols;
            cout << "\tr:" << r << "\tc:" << c << "\t" << meta.getTileName(layer_id, r, c) << endl; 
        }
    }
  }
  cout << "giga_image_meta ok" << endl;
  return 0;
}

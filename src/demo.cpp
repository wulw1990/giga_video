#include <iostream>
#include <cassert>
#include <algorithm>
using namespace std;

#include "giga_video.h"
#include "server/Player.hpp"

static int giga_image(int argc, char **argv);

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
  else {
    cerr << "main_internal_demo mode error: " << mode << endl;
    return -1;
  }
  return 0;
}
static int giga_image(int argc, char **argv) {
  if(argc < 2){
      cerr << "main_internal_demo giga_image args error." << endl;
      exit(-1);
  }
  Player player(argv[1], false);
  player.play();
  return 0;
  return 0;
}

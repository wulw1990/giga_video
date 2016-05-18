#include <iostream>
#include <cassert>
#include <algorithm>
using namespace std;

#include "giga_video.h"
#include "server/Player.hpp"

static int run(int argc, char **argv);

int main_internal_slave(int argc, char **argv) {
  if (argc < 2) {
    cerr << "main_internal_slave args error" << endl;
    return -1;
  }
  string mode = string(argv[1]);
  argc--;
  argv++;
  transform(mode.begin(), mode.end(), mode.begin(), ::tolower);
  cout << "mode : " << mode << endl;
  if (mode == "run")
    return run(argc, argv);
  else {
    cerr << "main_internal_slave mode error: " << mode << endl;
    return -1;
  }
  return 0;
}
static int run(int argc, char **argv) {
  if(argc < 2){
      cerr << "main_internal_slave run args error." << endl;
      exit(-1);
  }
  cout << "slave run" << endl;
  return 0;
}

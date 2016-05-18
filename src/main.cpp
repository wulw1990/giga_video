#include <iostream>
#include <cassert>
#include <algorithm>
using namespace std;

#include "giga_video.h"

int main(int argc, char **argv) {
  if (argc < 2) {
    cerr << "main args error" << endl;
    return -1;
  }
  string mode = string(argv[1]);
  argc--;
  argv++;
  transform(mode.begin(), mode.end(), mode.begin(), ::tolower);
  cout << "mode : " << mode << endl;
  if (mode == "demo")
    return main_internal_demo(argc, argv);
  if (mode == "construct")
    return main_internal_construct(argc, argv);
  if (mode == "test")
    return main_internal_test(argc, argv);
  if (mode == "master")
    return main_internal_master(argc, argv);
  if (mode == "slave")
    return main_internal_slave(argc, argv);
  if (mode == "client")
    return main_internal_client(argc, argv);
  else {
    cerr << "main mode error: " << mode << endl;
    return -1;
  }
  return 0;
}
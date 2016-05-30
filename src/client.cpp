#include <iostream>
#include <cassert>
#include <algorithm>
#include <thread>
#include <memory>
using namespace std;

#include <opencv2/opencv.hpp>
using namespace cv;

#include "giga_video.h"
#include "server/Player.hpp"
#include "Transmitter.hpp"
#include "Protocol.hpp"
#include "Client.hpp"


int main_internal_client(int argc, char **argv) {
  if (argc < 3) {
    cerr << "main_internal_client args error" << endl;
    return -1;
  }
  
  string server_ip(argv[1]);
  int server_socker_port = atoi(argv[2]);
  
  Client client(server_ip, server_socker_port);
  client.run();
  
  return 0;
}

#include "PlayerSlave.hpp"

using namespace std;
using namespace cv;

#include "Transmitter.hpp"

PlayerSlave::PlayerSlave(std::string path, int video_mode,
                         std::string server_ip, int server_port) {
  //
  m_client_id = Transmitter::initSocketClient(server_ip, server_port);
}
void PlayerSlave::play() {
  //
  while (1) {
    waitKey(30);
  }
}
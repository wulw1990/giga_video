#include "PlayerSlave.hpp"

using namespace std;
using namespace cv;

#include "Transmitter.hpp"
#include "Protocol.hpp"

PlayerSlave::PlayerSlave(std::string path, int video_mode,
                         std::string server_ip, int server_port) {
  // init socket
  m_client_id = Transmitter::initSocketClient(server_ip, server_port);
  // read window size
  std::vector<unsigned char> data;
  std::string cmd;
  int length;
  Transmitter::readData(m_client_id, data, Protocol::getHeadLen());
  Protocol::decodeHead(data, cmd, length);
  Transmitter::readData(m_client_id, data, length);
}
void PlayerSlave::play() {
  //
  while (1) {
    waitKey(30);
  }
}
#include "PlayerClient.hpp"

using namespace std;
using namespace cv;

#include "Timer.hpp"

#include "Transmitter.hpp"
#include "Protocol.hpp"

PlayerClient::PlayerClient(std::string server_ip, int server_port) {
  m_client_id = Transmitter::initSocketClient(server_ip, server_port);
  m_window_title = "Giga Player Client";
  m_info.x = 0;
  m_info.y = 0;
  m_info.z = 0;
  cout << "PlayerClient::init OK" << endl;
}
void PlayerClient::play() { //
  cout << "PlayerClient::play OK" << endl;

  while (1) {
    vector<unsigned char> buf;
    std::string cmd;
    int length;
    if (!Transmitter::readData(m_client_id, buf, Protocol::getHeadLen())) {
      break;
    }
    Protocol::decodeHead(buf, cmd, length);
    if (!Transmitter::readData(m_client_id, buf, length)) {
      break;
    }
    Mat frame = imdecode(buf, 1);
    imshow(m_window_title, frame);
    setMouseCallback(m_window_title, onMouse, &m_info);
    waitKey(1);

    Protocol::encodePoint3d(buf, m_info.x, m_info.y, m_info.z);
    if (!Transmitter::sendData(m_client_id, buf)) {
      break;
    }
    if (m_info.x != 0 || m_info.y != 0 || m_info.z != 0) {
      m_info.x = 0;
      m_info.y = 0;
      m_info.z = 0;
    }
  }
}
void PlayerClient::onMouse(int event, int x, int y, int, void *data) {
  if (event != EVENT_LBUTTONDOWN && event != EVENT_LBUTTONUP &&
      event != EVENT_LBUTTONDBLCLK && event != EVENT_RBUTTONDOWN &&
      event != EVENT_RBUTTONUP && event != EVENT_RBUTTONDBLCLK &&
      event != EVENT_MOUSEMOVE)
    return;
  Info *info = (Info *)data;
  switch (event) {
  case EVENT_LBUTTONDOWN:
    info->pre_x = x;
    info->pre_y = y;
    break;
  case EVENT_LBUTTONUP:
    info->x = info->pre_x - x;
    info->y = info->pre_y - y;
    break;
  case EVENT_LBUTTONDBLCLK:
    info->z = 1;
    break;
  case EVENT_RBUTTONDBLCLK:
    info->z = -1;
    break;
  }
}
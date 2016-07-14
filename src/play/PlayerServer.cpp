#include "PlayerServer.hpp"

#include <thread>
using namespace std;
using namespace cv;

#include "WaiterServer.hpp"
#include "Timer.hpp"

#include "Transmitter.hpp"
#include "Protocol.hpp"

const int SHOW_FPS = 15;
const int SHOW_MS = 1000 / SHOW_FPS;

PlayerServer::PlayerServer(std::string path, int video_mode, int port_slave,
                           int port_client) {
  cout << "PlayerServer::init begin" << endl;
  m_window_width = 1024;
  m_window_height = 720;
  m_window_title = "Giga Player Server";

  m_info.m_waiter = make_shared<WaiterServer>(
      path, cv::Size(m_window_width, m_window_height), video_mode, port_slave);

  Mat frame;
  if (m_info.m_waiter->hasFrame()) {
    m_info.m_waiter->getFrame(frame);
  }

  imshow(m_window_title, frame);
  setMouseCallback(m_window_title, onMouse, &m_info);
  waitKey(1);

  m_server_id = Transmitter::initSocketServer(port_client);

  cout << "PlayerServer::init ok" << endl;
}
void PlayerServer::play() {
  Timer timer;

  int client_id;
  string client_info;
  if (Transmitter::getClientId(m_server_id, client_id, client_info)) {
    cout << "WaiterServer::work_listen: " << client_info << endl;

    while (1) {
      timer.reset();
      Mat frame;
      if (m_info.m_waiter->hasFrame()) {
        m_info.m_waiter->getFrame(frame);
      }

      // send frame
      {
        vector<unsigned char> buf;
        vector<unsigned char> jpg;

        imencode(".jpg", frame, jpg);
        Protocol::encodeData(buf, "jpg", jpg);
        if (!Transmitter::sendData(client_id, buf)) {
          break;
        }
      }

      // read data
      {
        vector<unsigned char> buf;
        std::string cmd;
        int length;
        if (!Transmitter::readData(client_id, buf, Protocol::getHeadLen())) {
          break;
        }
        Protocol::decodeHead(buf, cmd, length);
        if (!Transmitter::readData(client_id, buf, length)) {
          break;
        }
        double x, y, z;
        Protocol::decodePoint3d(buf, x, y, z);
        if (abs(x) > 1e-6 || abs(y) > 1e-6 || abs(z) > 1e-6) {
          cout << "xyz: " << x << " " << y << " " << z << endl;
        }
        if (abs(x) > 1e-6 || abs(y) > 1e-6) {
          m_info.m_waiter->move(x, y);
        }
        if (abs(z) > 1e-6) {
          m_info.m_waiter->zoom(z);
        }
      }

      imshow(m_window_title, frame);
      setMouseCallback(m_window_title, onMouse, &m_info);

      int time = timer.getTimeUs() / 1000;
      int wait = max(1, SHOW_MS - time);
      char key = waitKey(wait);
    }
  }
  cout << "PlayerServer::play OK" << endl;
}
void PlayerServer::onMouse(int event, int x, int y, int, void *data) {
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
    info->m_waiter->move(info->pre_x - x, info->pre_y - y);
    break;
  case EVENT_LBUTTONDBLCLK:
    info->m_waiter->zoom(1);
    break;
  case EVENT_RBUTTONDBLCLK:
    info->m_waiter->zoom(-1);
    break;
  }
}

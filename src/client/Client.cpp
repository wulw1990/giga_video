#include "Client.hpp"
#include "Timer.hpp"
#include "Transmitter.hpp"
#include "Protocol.hpp"

using namespace std;
using namespace cv;

Client::Client(std::string server_ip, int server_socker_port) {
  //
  string video_name = "http://" + server_ip + ":8090/test.rm";
  cout << "video_name: " << video_name << endl;
  m_capture.open(video_name);
  assert(m_capture.isOpened());

  m_server_socket_id =
      Transmitter::initSocketClient(server_ip, server_socker_port);

  m_info.win_title = "giga player";
  m_info.mouse_x = -100;
  m_info.mouse_y = -100;
  m_info.mouse_color = Scalar(255, 255, 255);
  m_info.dx = 0;
  m_info.dy = 0;
  m_info.dz = 0;
  cout << "Player: m_info init ok" << endl;
}
void Client::run() {
  //
  const int MS = 66;
  Timer timer;
  while (1) {
    timer.reset();
    m_capture.read(m_info.frame);
    if (m_info.frame.empty()) {
      break;
    }
    imshow(m_info.win_title, m_info.frame);
    setMouseCallback(m_info.win_title, onMouse, &m_info);

    if (m_info.dx != 0 || m_info.dy != 0 || m_info.dz != 0) {
      cout << "send: " << m_info.dx << " " << m_info.dy << " " << m_info.dz
           << endl;
      vector<unsigned char> send_buf;
      Protocol::encodeXYZ(send_buf, m_info.dx, m_info.dy, m_info.dz);
      if (!Transmitter::sendData(m_server_socket_id, send_buf)) {
        cout << "connect end" << endl;
        exit(-1);
      }
      m_info.dx = 0;
      m_info.dy = 0;
      m_info.dz = 0;
    }

    int time = timer.getTimeUs() / 1000;
    cout << "Client time: " << time << endl;
    // endl;
    int wait = max(1, MS - time);
    char key = waitKey(wait);
    if (key == 'q')
      break;
  }
}
void Client::drawMouse(cv::Mat &show, int x, int y, Scalar color) {
  std::vector<cv::Point> fillContSingle;
  fillContSingle.push_back(cv::Point(x, y));
  fillContSingle.push_back(cv::Point(x + 0, y + 20));
  fillContSingle.push_back(cv::Point(x + 5, y + 15));
  fillContSingle.push_back(cv::Point(x + 15, y + 25));
  fillContSingle.push_back(cv::Point(x + 25, y + 15));
  fillContSingle.push_back(cv::Point(x + 15, y + 5));
  fillContSingle.push_back(cv::Point(x + 20, y + 0));
  std::vector<std::vector<cv::Point>> fillContAll;
  fillContAll.push_back(fillContSingle);
  cv::fillPoly(show, fillContAll, color);
  // circle(show, Point2f(x, y), 10, Scalar(255, 0, 0), -1);
}
void Client::onMouse(int event, int x, int y, int, void *data) {
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
    info->mouse_color = Scalar(0, 0, 255);
    break;
  case EVENT_LBUTTONUP:
    // info->m_waiter->move(info->pre_x - x, info->pre_y - y);
    info->dx = info->pre_x - x;
    info->dy = info->pre_y - y;
    info->mouse_color = Scalar(255, 255, 255);
    cout << "EVENT_LBUTTONUP" << rand() << endl;
    break;
  case EVENT_LBUTTONDBLCLK:
    cout << "EVENT_LBUTTONDBLCLK" << rand() << endl;
    // info->m_waiter->zoom(1);
    info->dz = 1;
    break;
  case EVENT_RBUTTONDOWN:
    info->mouse_color = Scalar(255, 0, 0);
    break;
  case EVENT_RBUTTONUP:
    info->mouse_color = Scalar(255, 255, 255);
    break;
  case EVENT_RBUTTONDBLCLK:
    // info->m_waiter->zoom(-1);
    cout << "EVENT_RBUTTONDBLCLK" << rand() << endl;
    info->dz = -1;
    break;
  case EVENT_MOUSEMOVE:
    info->mouse_x = x;
    info->mouse_y = y;
    break;
  }
  info->show = info->frame.clone();
  drawMouse(info->show, info->mouse_x, info->mouse_y, info->mouse_color);
  imshow(info->win_title, info->show);
}

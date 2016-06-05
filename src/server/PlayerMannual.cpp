#include "PlayerMannual.hpp"

#include <thread>
using namespace std;
using namespace cv;

#include "WaiterServer.hpp"
#include "Timer.hpp"

const int SHOW_FPS = 15;
const int SHOW_MS = 1000 / SHOW_FPS;

static void work_record(string video_name, cv::Mat &show, bool &record_end,
                        mutex &locker) {
  const int FPS = 30;
  const int MS = 1000 / FPS;
  VideoWriter writer;
  if (video_name != "") {
    writer.open(video_name, CV_FOURCC('M', 'J', 'P', 'G'), FPS, show.size());
    assert(writer.isOpened());
  }
  Timer timer;
  while (!record_end) {
    timer.reset();

    locker.lock();
    Mat tmp = show.clone();
    locker.unlock();
    writer << tmp;

    int time = (int)(timer.getTimeUs() / 1000);
    // cout << "time : " << time << endl;

    int wait = max(1, MS - time);
    std::this_thread::sleep_for(std::chrono::milliseconds(wait));
  }
}

PlayerMannual::PlayerMannual(std::string path, int video_mode,
                             string output_video) {
  m_window_width = 1024;
  m_window_height = 720;

  m_info.m_waiter = make_shared<WaiterServer>(
      path, Size(m_window_width, m_window_height), video_mode);
  m_info.win_title = "giga player";
  m_info.mouse_x = -100;
  m_info.mouse_y = -100;
  m_info.mouse_color = Scalar(255, 255, 255);
  cout << "PlayerMannual: m_info init ok" << endl;

  // init show
  // if (m_info.m_waiter->hasFrame()) {
  //   m_info.m_waiter->getFrame(m_info.frame);
  // }
  // m_info.show_locker.lock();
  // m_info.show = m_info.frame.clone();
  // m_info.show_locker.unlock();
  // drawMouse(m_info.show, m_info.mouse_x, m_info.mouse_y, m_info.mouse_color);
  // imshow(m_info.win_title, m_info.show);
  // setMouseCallback(m_info.win_title, onMouse, &m_info);

  m_record_end = false;
  m_record_thread = thread(work_record, output_video, ref(m_info.show),
                           ref(m_record_end), ref(m_info.show_locker));
}
void PlayerMannual::drawMouse(cv::Mat &show, int x, int y, Scalar color) {
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
void PlayerMannual::play() {
  cout << "hh" << endl;
  cvNamedWindow(m_info.win_title.c_str(), CV_WINDOW_NORMAL);
  cvSetWindowProperty(m_info.win_title.c_str(), CV_WND_PROP_FULLSCREEN,
                      CV_WINDOW_FULLSCREEN);

  Timer timer;
  while (1) {
    timer.reset();
    if (m_info.m_waiter->hasFrame()) {
      m_info.m_waiter->getFrame(m_info.frame);
      // imwrite("../refer.png", m_info.frame);
      // cout << rand() << endl;
    }
    m_info.show_locker.lock();
    m_info.show = m_info.frame.clone();
    m_info.show_locker.unlock();
    drawMouse(m_info.show, m_info.mouse_x, m_info.mouse_y, m_info.mouse_color);

    cvNamedWindow(m_info.win_title.c_str(), CV_WINDOW_NORMAL);
    cvSetWindowProperty(m_info.win_title.c_str(), CV_WND_PROP_FULLSCREEN,
                        CV_WINDOW_FULLSCREEN);
    imshow(m_info.win_title, m_info.show);
    setMouseCallback(m_info.win_title, onMouse, &m_info);

    int time = timer.getTimeUs() / 1000;
    // cout << "PlayerMannual time: " << time << " index: " <<
    // m_info.thumnail_index <<
    // endl;
    int wait = max(1, SHOW_MS - time);
    char key = waitKey(wait);
    if (key == 'q')
      break;
  }

  m_record_end = true;
  m_record_thread.join();
}
void PlayerMannual::onMouse(int event, int x, int y, int, void *data) {
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
    info->m_waiter->move(info->pre_x - x, info->pre_y - y);
    info->mouse_color = Scalar(255, 255, 255);
    break;
  case EVENT_LBUTTONDBLCLK:
    info->m_waiter->zoom(1);
    break;
  case EVENT_RBUTTONDOWN:
    info->mouse_color = Scalar(255, 0, 0);
    break;
  case EVENT_RBUTTONUP:
    info->mouse_color = Scalar(255, 255, 255);
    break;
  case EVENT_RBUTTONDBLCLK:
    info->m_waiter->zoom(-1);
    break;
  case EVENT_MOUSEMOVE:
    info->mouse_x = x;
    info->mouse_y = y;
    break;
  }
}

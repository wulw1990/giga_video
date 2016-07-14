#include "PlayerAuto.hpp"

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

PlayerAuto::PlayerAuto(std::string path, int video_mode, int port, string output_video) {
  cout << "PlayerAuto::init begin" << endl;
  m_window_width = 1024;
  m_window_height = 720;

  m_info.m_waiter = make_shared<WaiterServer>(
      path, cv::Size(m_window_width, m_window_height), video_mode, port);
  m_info.win_title = "giga PlayerAuto";
  m_info.mouse_x = -100;
  m_info.mouse_y = -100;
  m_info.mouse_color = Scalar(255, 255, 255);
  m_info.thumnail_index = -1;

  // getThumbnail
  if (m_info.m_waiter->hasThumbnail())
    updateThumbnail();

  // init show
  // cvNamedWindow(m_info.win_title.c_str(), CV_WINDOW_NORMAL);
  // cvSetWindowProperty(m_info.win_title.c_str(), CV_WND_PROP_FULLSCREEN,
  //                     CV_WINDOW_FULLSCREEN);

  if (m_info.m_waiter->hasFrame()) {
    m_info.m_waiter->getFrame(m_info.frame);
  }
  m_info.show_locker.lock();
  m_info.show = m_info.frame.clone();
  m_info.show.push_back(m_info.thumnail_show);
  m_info.show_locker.unlock();

  drawMouse(m_info.show, m_info.mouse_x, m_info.mouse_y, m_info.mouse_color);
  imshow(m_info.win_title, m_info.show);
  setMouseCallback(m_info.win_title, onMouse, &m_info);

  m_record_end = false;
  m_record_thread = thread(work_record, output_video, ref(m_info.show),
                           ref(m_record_end), ref(m_info.show_locker));
  cout << "PlayerAuto::init ok" << endl;
}
void PlayerAuto::updateThumbnail() {
  // cout << "thumnail begin" << endl;
  vector<Mat> thumnail;
  m_info.m_waiter->getThumbnail(thumnail);
  // cout << "thumnail size: " << thumnail.size() << endl;
  if (!thumnail.empty()) {
    int rows = thumnail[0].rows;
    int cols = thumnail[0].cols;
    for (size_t i = 0; i < thumnail.size(); ++i) {
      resize(thumnail[i], thumnail[i], Size(cols, rows));
      Rect rect(0, 0, cols, rows);
      rectangle(thumnail[i], rect, Scalar(255, 0, 0), 5);
    }
    m_info.thumnail_show = Mat(rows, cols * thumnail.size(), CV_8UC3);
    m_info.thumnail_rect.clear();
    for (size_t i = 0; i < thumnail.size(); ++i) {
      Rect rect(cols * i, 0, cols, rows);
      thumnail[i].copyTo(m_info.thumnail_show(rect));
      m_info.thumnail_rect.push_back(rect);
    }

    int thumnail_show_cols_max = m_window_width;
    int thumnail_show_rows_max = m_window_height / 10;
    float scale = (float)thumnail_show_cols_max / m_info.thumnail_show.cols;
    int thumnail_show_cols = thumnail_show_cols_max;
    int thumnail_show_rows = scale * m_info.thumnail_show.rows;

    if (thumnail_show_rows > thumnail_show_rows_max) {
      scale *= (float)thumnail_show_rows_max / thumnail_show_rows;
      thumnail_show_cols = m_info.thumnail_show.cols * scale;
      thumnail_show_rows = m_info.thumnail_show.rows * scale;
      thumnail_show_cols = min(thumnail_show_cols, thumnail_show_cols_max);
      thumnail_show_rows = min(thumnail_show_rows, thumnail_show_rows_max);
      Mat tmp = m_info.thumnail_show.clone();
      resize(tmp, tmp, Size(thumnail_show_cols, thumnail_show_rows));
      m_info.thumnail_show = Mat(thumnail_show_rows_max, thumnail_show_cols_max,
                                 CV_8UC3, Scalar(0, 0, 0));
      tmp.copyTo(m_info.thumnail_show(Rect(0, 0, tmp.cols, tmp.rows)));
    } else {
      resize(m_info.thumnail_show, m_info.thumnail_show,
             Size(thumnail_show_cols, thumnail_show_rows));
    }

    for (size_t i = 0; i < thumnail.size(); ++i) {
      m_info.thumnail_rect[i].x = scale * m_info.thumnail_rect[i].x;
      m_info.thumnail_rect[i].y = scale * m_info.thumnail_rect[i].y;
      m_info.thumnail_rect[i].width = scale * m_info.thumnail_rect[i].width;
      m_info.thumnail_rect[i].height = scale * m_info.thumnail_rect[i].height;
      // cout << m_info.thumnail_rect[i] << endl;
    }
  }
  // cout << "PlayerAuto: thumnail ok" << endl;
}

void PlayerAuto::drawMouse(cv::Mat &show, int x, int y, Scalar color) {
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
void PlayerAuto::play() {
  Timer timer;
  while (1) {
    timer.reset();
    if (m_info.thumnail_index >= 0) {
      // cout << m_info.thumnail_index << endl;
      m_info.m_waiter->setThumbnailIndex(m_info.thumnail_index);
      m_info.thumnail_index = -1;
    }
    if (m_info.m_waiter->hasThumbnail()) {
      updateThumbnail();
    }
    if (m_info.m_waiter->hasFrame()) {
      m_info.m_waiter->getFrame(m_info.frame);
      // imwrite("../refer.png", m_info.frame);
      // cout << rand() << endl;
    }
    m_info.show_locker.lock();
    m_info.show = m_info.frame.clone();
    m_info.show.push_back(m_info.thumnail_show);
    m_info.show_locker.unlock();
    drawMouse(m_info.show, m_info.mouse_x, m_info.mouse_y, m_info.mouse_color);
    imshow(m_info.win_title, m_info.show);
    setMouseCallback(m_info.win_title, onMouse, &m_info);

    int time = timer.getTimeUs() / 1000;
    // cout << "PlayerAuto time: " << time << " index: " <<
    // m_info.thumnail_index <<
    // endl;
    int wait = max(1, SHOW_MS - time);
    char key = waitKey(wait);
    // if (key == 'q')
    //   break;
  }

  m_record_end = true;
  m_record_thread.join();
}
int PlayerAuto::getThunbmailIndex(int x, int y,
                                  const std::vector<cv::Rect> &rect_vec) {
  for (size_t i = 0; i < rect_vec.size(); ++i) {
    Rect rect = rect_vec[i];
    if (x >= rect.x && x < rect.x + rect.width && y >= rect.y &&
        y < rect.y + rect.height) {
      return i;
    }
  }
  return -1;
}
void PlayerAuto::onMouse(int event, int x, int y, int, void *data) {
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
    info->thumnail_index = -1;
    break;
  case EVENT_LBUTTONUP:
    info->m_waiter->move(info->pre_x - x, info->pre_y - y);
    info->mouse_color = Scalar(255, 255, 255);
    break;
  case EVENT_LBUTTONDBLCLK:
    info->thumnail_index =
        getThunbmailIndex(x, y - info->frame.rows, info->thumnail_rect);
    if (info->thumnail_index < 0)
      info->m_waiter->zoom(1);
    // cout << "ThunbmailIndex: " << info->thumnail_index << endl;
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
  info->show_locker.lock();
  info->show = info->frame.clone();
  info->show.push_back(info->thumnail_show);
  drawMouse(info->show, info->mouse_x, info->mouse_y, info->mouse_color);
  info->show_locker.unlock();
  imshow(info->win_title, info->show);
  waitKey(1);
}

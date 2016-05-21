#include "Player.hpp"
using namespace std;
using namespace cv;

#include "WaiterServer.hpp"
#include "Timer.hpp"

const string win_title = "giga player";
const int w = 1700;
const int h = 900;

const int FPS = 15;
const int MS = 1000 / FPS;

Player::Player(std::string path, int video_mode, string output_video) {
  m_info.m_waiter = make_shared<WaiterServer>(path, w, h, video_mode);
  m_output_video = output_video;
}
void Player::play() {
  //   cvNamedWindow(win_title.c_str(), CV_WINDOW_NORMAL);
  //   cvSetWindowProperty(win_title.c_str(), CV_WND_PROP_FULLSCREEN,
  //   CV_WINDOW_FULLSCREEN);

  VideoWriter video_writer;
  if (m_output_video != "") {
    video_writer.open(m_output_video, CV_FOURCC('M', 'J', 'P', 'G'), FPS,
                      Size(w, h));
    assert(video_writer.isOpened());
  }

  Timer timer;
  Mat frame;
  while (1) {
    timer.reset();
    if (m_info.m_waiter->hasFrame()) {
      Timer frame_timer;
      frame_timer.reset();
      m_info.m_waiter->getFrame(frame);
      //   cout << "Frame Time: " << frame_timer.getTimeUs() / 1000 << " ms" <<
      //   endl;
      imshow(win_title, frame);
      setMouseCallback(win_title, onMouse, &m_info);
    }

    video_writer << frame;
    int time = timer.getTimeUs() / 1000;
    cout << "time: " << time << endl;
    int wait = max(1, 66 - time);
    char key = waitKey(wait);
    if (key == 'q')
      break;
  }
}
void Player::onMouse(int event, int x, int y, int, void *data) {
  if (event != EVENT_LBUTTONDOWN && event != EVENT_LBUTTONUP &&
      event != EVENT_LBUTTONDBLCLK && event != EVENT_RBUTTONDBLCLK)
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
    // cout << "EVENT_LBUTTONDBLCLK" << endl;
    break;
  case EVENT_RBUTTONDBLCLK:
    info->m_waiter->zoom(-1);
    break;
  }
}

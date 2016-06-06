#include "WindowProviderRemote.hpp"

using namespace std;
using namespace cv;

#include "FrameProvider.hpp"
#include "Timer.hpp"
#include "Transmitter.hpp"
#include "Protocol.hpp"

static void work_receive(int socket_id, bool &has_frame, Mat &frame, Mat &mask,
                         mutex &locker) {
  while (1) {
    vector<unsigned char> buf;
    std::string cmd;
    int length;
    if (!Transmitter::readData(socket_id, buf, Protocol::getHeadLen())) {
      cerr << "Connect End" << endl;
      exit(-1);
    }
    Protocol::decodeHead(buf, cmd, length);
    cout << "cmd: " << cmd << endl;
    cout << "length: " << length << endl;
    if (!Transmitter::readData(socket_id, buf, length)) {
      cerr << "Connect End" << endl;
      exit(-1);
    }
    Mat tmp_frame = imdecode(buf, 1);
    cout << "tmp_frame size: " << tmp_frame.size() << endl;

    if (!Transmitter::readData(socket_id, buf, Protocol::getHeadLen())) {
      cerr << "Connect End" << endl;
      exit(-1);
    }
    Protocol::decodeHead(buf, cmd, length);
    cout << "cmd: " << cmd << endl;
    if (!Transmitter::readData(socket_id, buf, length)) {
      cerr << "Connect End" << endl;
      exit(-1);
    }
    Mat tmp_mask = imdecode(buf, 1);
    cvtColor(tmp_mask, tmp_mask, CV_BGR2GRAY);
    cout << "tmp_mask size: " << tmp_mask.size() << endl;

    locker.lock();
    frame = tmp_frame;
    mask = tmp_mask;
    has_frame = true;
    locker.unlock();
    cout << "new frame" << endl;
  }
}

WindowProviderRemote::WindowProviderRemote(int socket_id,
                                           cv::Size window_size) {
  m_has_thumnail = false;
  m_has_frame = false;
  m_socket_id = socket_id;

  // send window_size
  {
    vector<unsigned char> data;
    Protocol::encodeSize(data, window_size.width, window_size.height);
    Transmitter::sendData(socket_id, data);
  }

  // read thumnail and position
  {
    vector<unsigned char> buf;
    std::string cmd;
    int length;
    Transmitter::readData(socket_id, buf, Protocol::getHeadLen());
    // cout << "buf size: " << buf.size() << endl;
    Protocol::decodeHead(buf, cmd, length);
    // cout << "cmd: " << cmd << endl;
    // cout << "length: " << length << endl;
    Transmitter::readData(socket_id, buf, length);
    int n_thumbnails;
    Protocol::decodeLength(buf, n_thumbnails);
    cout << "n_thumbnails: " << n_thumbnails << endl;

    m_thumbnail.resize(n_thumbnails);
    m_position.resize(n_thumbnails);
    for (int i = 0; i < n_thumbnails; ++i) {
      Transmitter::readData(socket_id, buf, Protocol::getHeadLen());
      Protocol::decodeHead(buf, cmd, length);
      // cout << "cmd: " << cmd << endl;
      // cout << "length: " << length << endl;
      Transmitter::readData(socket_id, buf, length);
      m_thumbnail[i] = imdecode(buf, 1);
      // imshow("thumbnail", m_thumbnail[i]);
      // waitKey(0);
    }
    for (int i = 0; i < n_thumbnails; ++i) {
      Transmitter::readData(socket_id, buf, Protocol::getHeadLen());
      Protocol::decodeHead(buf, cmd, length);
      // cout << "cmd: " << cmd << endl;
      // cout << "length: " << length << endl;
      Transmitter::readData(socket_id, buf, length);
      Protocol::decodePoint3d(buf, m_position[i].x, m_position[i].y,
                              m_position[i].z);
      cout << m_position[i] << endl;
    }
  }
  // new thread
  m_has_thumnail = true;
  m_thread_receive = thread(work_receive, socket_id, ref(m_has_frame),
                            ref(m_frame), ref(m_mask), ref(m_locker));
}
void WindowProviderRemote::setWindowPosition(cv::Point3d position) {
  // send position
  cout << "WindowProviderRemote:position:" << position << endl;
  vector<unsigned char> data;
  Protocol::encodePoint3d(data, position.x, position.y, position.z);
  Transmitter::sendData(m_socket_id, data);
}
bool WindowProviderRemote::hasFrame() {
  // m_has_frame updated by sub thread
  return m_has_frame;
}
void WindowProviderRemote::getFrame(cv::Mat &frame, cv::Mat &mask) {
  // frame mask update by sub thread
  m_locker.lock();
  Mat tmp_frame = m_frame;
  Mat tmp_mask = m_mask;
  m_locker.unlock();

  frame = tmp_frame.clone();
  mask = tmp_mask.clone();

  m_has_frame = false;
}
bool WindowProviderRemote::hasThumbnail() {
  //
  return m_has_thumnail;
}
void WindowProviderRemote::getThumbnail(std::vector<cv::Mat> &thumbnail,
                                        std::vector<cv::Point3d> &position) {
  thumbnail = m_thumbnail;
  position = m_position;
  m_has_thumnail = false;
}
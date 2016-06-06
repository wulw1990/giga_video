#include "WindowProviderRemote.hpp"

using namespace std;
using namespace cv;

#include "FrameProvider.hpp"
#include "Timer.hpp"
#include "Transmitter.hpp"
#include "Protocol.hpp"

void WindowProviderRemote::work_receive(Info &info) {
  while (1) {
    Mat tmp_frame, tmp_mask;

    // read frame
    {
      vector<unsigned char> buf;
      std::string cmd;
      int length;
      if (!Transmitter::readData(info.socket_id, buf, Protocol::getHeadLen())) {
        info.die();
        break;
      }
      Protocol::decodeHead(buf, cmd, length);
      if (!Transmitter::readData(info.socket_id, buf, length)) {
        info.die();
        break;
      }
      tmp_frame = imdecode(buf, 1);
    }

    // read mask
    {
      vector<unsigned char> buf;
      std::string cmd;
      int length;
      if (!Transmitter::readData(info.socket_id, buf, Protocol::getHeadLen())) {
        info.die();
        break;
      }
      Protocol::decodeHead(buf, cmd, length);
      if (!Transmitter::readData(info.socket_id, buf, length)) {
        info.die();
        break;
      }
      tmp_mask = imdecode(buf, 0);
    }

    info.locker.lock();
    info.frame = tmp_frame;
    info.mask = tmp_mask;
    info.has_frame = true;
    info.locker.unlock();
  }
}

WindowProviderRemote::WindowProviderRemote(int socket_id, cv::Size window_size,
                                           std::string name) {
  m_info.socket_id = socket_id;
  m_info.window_size = window_size;
  m_info.name = name;

  // send window_size
  {
    vector<unsigned char> data;
    Protocol::encodeSize(data, window_size.width, window_size.height);
    if (!Transmitter::sendData(socket_id, data)) {
      m_info.die();
      return;
    }
  }

  // read thumnail and position
  {
    vector<unsigned char> buf;
    std::string cmd;
    int length;
    if (!Transmitter::readData(socket_id, buf, Protocol::getHeadLen())) {
      m_info.die();
      return;
    }
    Protocol::decodeHead(buf, cmd, length);
    if (!Transmitter::readData(socket_id, buf, length)) {
      m_info.die();
      return;
    }
    int n_thumbnails;
    Protocol::decodeLength(buf, n_thumbnails);
    m_info.thumbnail.resize(n_thumbnails);
    m_info.position.resize(n_thumbnails);
    for (int i = 0; i < n_thumbnails; ++i) {
      if (!Transmitter::readData(socket_id, buf, Protocol::getHeadLen())) {
        m_info.die();
        return;
      }
      Protocol::decodeHead(buf, cmd, length);
      if (!Transmitter::readData(socket_id, buf, length)) {
        m_info.die();
        return;
      }
      m_info.thumbnail[i] = imdecode(buf, 1);
    }
    for (int i = 0; i < n_thumbnails; ++i) {
      if (!Transmitter::readData(socket_id, buf, Protocol::getHeadLen())) {
        m_info.die();
        return;
      }
      Protocol::decodeHead(buf, cmd, length);
      if (!Transmitter::readData(socket_id, buf, length)) {
        m_info.die();
        return;
      }
      Protocol::decodePoint3d(buf, m_info.position[i].x, m_info.position[i].y,
                              m_info.position[i].z);
    }
    m_info.has_thumnail = true;
  }

  // new thread
  m_thread_receive = thread(work_receive, ref(m_info));
}
void WindowProviderRemote::setWindowPosition(cv::Point3d position) {
  if (m_info.is_dead) {
    return;
  }
  vector<unsigned char> data;
  Protocol::encodePoint3d(data, position.x, position.y, position.z);
  if (!Transmitter::sendData(m_info.socket_id, data)) {
    m_info.die();
    return;
  }
}
bool WindowProviderRemote::hasFrame() { return m_info.has_frame; }
void WindowProviderRemote::getFrame(cv::Mat &frame, cv::Mat &mask) {
  m_info.locker.lock();
  Mat tmp_frame = m_info.frame;
  Mat tmp_mask = m_info.mask;
  m_info.locker.unlock();

  frame = tmp_frame.clone();
  mask = tmp_mask.clone();
}
bool WindowProviderRemote::hasThumbnail() { return m_info.has_thumnail; }
void WindowProviderRemote::getThumbnail(std::vector<cv::Mat> &thumbnail,
                                        std::vector<cv::Point3d> &position) {
  thumbnail = m_info.thumbnail;
  position = m_info.position;
  m_info.has_thumnail = false;
}
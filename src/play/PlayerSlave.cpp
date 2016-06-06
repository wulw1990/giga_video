#include "PlayerSlave.hpp"

using namespace std;
using namespace cv;

#include "Transmitter.hpp"
#include "Protocol.hpp"
#include "FrameProvider.hpp"
#include "Timer.hpp"

static void work_receive(int socket_id, Point3d &position, mutex &locker) {
  while (1) {
    vector<unsigned char> buf;
    std::string cmd;
    int length;
    Transmitter::readData(socket_id, buf, Protocol::getHeadLen());
    Protocol::decodeHead(buf, cmd, length);
    // cout << "cmd: " << cmd << endl;
    // cout << "length: " << length << endl;
    Transmitter::readData(socket_id, buf, length);

    Point3d tmp_position;
    Protocol::decodePoint3d(buf, tmp_position.x, tmp_position.y,
                            tmp_position.z);

    locker.lock();
    position = tmp_position;
    locker.unlock();

    cout << "position: " << position << endl;
  }
}

PlayerSlave::PlayerSlave(std::string path, int mode_video,
                         std::string server_ip, int server_port) {
  // init frame provider
  m_provider = make_shared<FrameProvider>(path, mode_video);
  m_position = Point3d(0.5, 0.5, 0);

  // init socket
  m_client_id = Transmitter::initSocketClient(server_ip, server_port);

  // read window size
  {
    std::vector<unsigned char> data;
    std::string cmd;
    int length;
    Transmitter::readData(m_client_id, data, Protocol::getHeadLen());
    Protocol::decodeHead(data, cmd, length);
    // cout << "cmd: " << cmd << endl;
    // cout << "length: " << length << endl;
    Transmitter::readData(m_client_id, data, length);
    int width, height;
    Protocol::decodeSize(data, width, height);
    // cout << "width: " << width << endl;
    // cout << "height: " << height << endl;
    m_size.width = width;
    m_size.height = height;
    cout << "PlayerSlave:m_size:" << m_size << endl;
  }

  // send thumbnail
  {
    vector<Mat> thumbnail;
    vector<Point3d> position;
    m_provider->getThumbnail(thumbnail, position);
    cout << "thumbnail size: " << thumbnail.size() << endl;
    cout << "position size: " << position.size() << endl;

    std::vector<unsigned char> buf;
    Protocol::encodeLength(buf, thumbnail.size());
    Transmitter::sendData(m_client_id, buf);

    vector<unsigned char> jpg;
    for (size_t i = 0; i < thumbnail.size(); ++i) {
      imencode(".jpg", thumbnail[i], jpg);
      Mat img = imdecode(jpg, 1);
      // imshow("img", img);
      // waitKey(0);
      Protocol::encodeData(buf, "jpg", jpg);
      Transmitter::sendData(m_client_id, buf);
    }
    for (size_t i = 0; i < thumbnail.size(); ++i) {
      cout << "position: " << position[i] << endl;
      Protocol::encodePoint3d(buf, position[i].x, position[i].y, position[i].z);
      Transmitter::sendData(m_client_id, buf);
    }
  }
  m_thread_receive =
      thread(work_receive, m_client_id, ref(m_position), ref(m_locker));
}
void PlayerSlave::play() {
  const int MS = 66;
  Timer timer;
  while (1) {
    timer.reset();
    Mat frame = m_provider->getFrameBackground(m_size, m_position);
    Mat mask = Mat(frame.rows, frame.cols, CV_8UC1, 0);
    // cout << "Frame Time: " << timer.getTimeUs() / 1000 << " ms" << endl;
    bool need_update_foreground =
        m_provider->hasFrameForeground(m_size, m_position);
    if (need_update_foreground) {
      cv::Mat foregournd_frame;
      cv::Mat foregournd_mask;
      m_provider->getFrameForeground(m_size, m_position, foregournd_frame,
                                     foregournd_mask);
      foregournd_frame.copyTo(frame, foregournd_mask);
      mask = foregournd_mask;
    }

#if 1
    cout << "mask size" << mask.size() << endl;
    Mat mask_bgr;
    cvtColor(mask, mask_bgr, CV_GRAY2BGR);

// frame = frame.clone();
// mask = mask.clone();

// vector<unsigned char> buf;
// vector<unsigned char> jpg;
// imencode(".jpg", frame, jpg);
// Protocol::encodeData(buf, "jpg", jpg);
// Transmitter::sendData(m_client_id, buf);
// imencode(".jpg", mask, jpg);
// Protocol::encodeData(buf, "jpg", jpg);
// Transmitter::sendData(m_client_id, buf);
#endif
    imshow("frame", frame);
    int time = timer.getTimeUs() / 1000;
    int wait = max(1, MS - time);
    waitKey(wait);
  }
}
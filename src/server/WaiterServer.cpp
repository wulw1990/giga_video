#include "WaiterServer.hpp"

#include "FrameProvider.hpp"
#include "WindowController.hpp"
#include "Timer.hpp"
#include "WindowProviderLocal.hpp"

using namespace std;
using namespace cv;

WaiterServer::WaiterServer(std::string path, int window_width,
                           int window_height, int video_mode) {
  {
    FrameProvider provider(path, 0);
    int n_layers = provider.getNumLayers();
    Size top_layer_size(provider.getLayerWidth(n_layers - 1),
                        provider.getLayerHeight(n_layers - 1));
    Size winsize(window_width, window_height);
    m_window_controller =
        make_shared<WindowController>(n_layers, top_layer_size, winsize);
  }

  m_window_local = make_shared<WindowProviderLocal>(
      path, video_mode, window_width, window_height);
  m_has_frame = false;
  m_need_update_foreground = false;
  updateFrameBackground();
  updateFrameForeground();

  m_has_thumbnail = m_window_local->hasThumbnail();
  if (m_window_local->hasThumbnail())
    m_window_local->getThumbnail(m_thumbnail);
  if (m_thumbnail.empty())
    m_has_thumbnail = false;
  cout << "WaiterServer init ok" << endl;
}
void WaiterServer::move(float dx, float dy) {
  m_window_controller->move(dx, dy);
  updateFrameBackground();
  updateFrameForeground();
  m_has_frame = true;
}
void WaiterServer::zoom(float dz) {
  m_window_controller->zoom(dz);
  updateFrameBackground();
  updateFrameForeground();
  m_has_frame = true;
}
bool WaiterServer::hasFrame() {
  // hasFrame

  if (!m_auto_x.empty()) {
    m_window_controller->setXYZ(
        m_auto_x[m_auto_index], m_auto_y[m_auto_index], m_auto_z[m_auto_index]);

    m_auto_index++;
    if (m_auto_index == (int)m_auto_x.size()) {
      m_auto_x.clear();
      m_auto_y.clear();
      m_auto_z.clear();
    }
    // cout << "m_auto_index: " << m_auto_index << endl;
  }

  updateFrameBackground();
  updateFrameForeground();
  m_has_frame = true;

  return m_has_frame;
}
void WaiterServer::getFrame(cv::Mat &frame) {
  // getFrame
  frame = m_frame;
  m_has_frame = false;
}
void WaiterServer::getFrameMasked(cv::Mat &frame) {
  //
  frame = m_frame.clone();
  Mat black = Mat(frame.size(), frame.type(), Scalar(0, 0, 0));
  Mat mask = m_mask.clone();
  mask = Mat(mask.size(), mask.type(), Scalar(255)) - mask;
  black.copyTo(frame, mask);
  m_has_frame = false;
}
bool WaiterServer::hasThumbnail() {
  // hasThumbnail
  return m_has_thumbnail;
}
void WaiterServer::getThumbnail(std::vector<cv::Mat> &thumbnail) {
  // getThumbnail
  thumbnail = m_thumbnail;
}
void WaiterServer::getLinearPath(double sx, double sy, double sz, double dx,
                                 double dy, double dz, int len,
                                 std::vector<double> &vx,
                                 std::vector<double> &vy,
                                 std::vector<double> &vz) {
  //
  vx.resize(len);
  vy.resize(len);
  vz.resize(len);

  for (int i = 0; i < len; ++i) {
    vx[i] = sx + (dx - sx) / len * i;
    vy[i] = sy + (dy - sy) / len * i;
    vz[i] = sz + (dz - sz) / len * i;
  }
}
void WaiterServer::setThumbnailIndex(int index) {
  //
  vector<double> camera_x, camera_y, camera_z;
  // assert(
      // m_window_local.provider->getVideoPosition(camera_x, camera_y, camera_z));
  // for (size_t camera_id = 0; camera_id < camera_x.size(); ++camera_id) {
  //   cout << camera_id << " : " << camera_x[camera_id] << " "
  //        << camera_y[camera_id] << " " << camera_z[camera_id] << endl;
  // }
  if (index < 0 || index >= (int)camera_x.size()) {
    cout << "index error: " << index << endl;
    return;
  }
  m_auto_x.clear();
  m_auto_y.clear();
  m_auto_z.clear();
  m_auto_index = 0;

  double sx, sy, sz;
  m_window_controller->getXYZ(sx, sy, sz);

  double dx, dy, dz;
  dx = camera_x[index];
  dy = camera_y[index];
  dz = camera_z[index];

  const double mz = -0.5;
  vector<double> tx, ty, tz;

  getLinearPath(sx, sy, sz, sx, sy, mz, 15, tx, ty, tz);
  m_auto_x.insert(m_auto_x.end(), tx.begin(), tx.end());
  m_auto_y.insert(m_auto_y.end(), ty.begin(), ty.end());
  m_auto_z.insert(m_auto_z.end(), tz.begin(), tz.end());

  getLinearPath(sx, sy, mz, dx, dy, mz, 20, tx, ty, tz);
  m_auto_x.insert(m_auto_x.end(), tx.begin(), tx.end());
  m_auto_y.insert(m_auto_y.end(), ty.begin(), ty.end());
  m_auto_z.insert(m_auto_z.end(), tz.begin(), tz.end());

  getLinearPath(dx, dy, mz, dx, dy, dz, 15, tx, ty, tz);
  m_auto_x.insert(m_auto_x.end(), tx.begin(), tx.end());
  m_auto_y.insert(m_auto_y.end(), ty.begin(), ty.end());
  m_auto_z.insert(m_auto_z.end(), tz.begin(), tz.end());

  m_auto_x.push_back(dx);
  m_auto_y.push_back(dy);
  m_auto_z.push_back(dz);

  // for(size_t i=0; i<m_auto_x.size(); ++i){
  //   cout << i << ": " << m_auto_x[i] << " " << m_auto_y[i] << " " <<
  //   m_auto_z[i] << endl;
  // }
  // exit(-1);
}

void WaiterServer::updateFrameBackground() {
  #if 0
  double x, y, z;
  m_window_controller->getXYZ(x, y, z);
  // cout << "x: " << x << "\ty: " << y << "\tz: " << z << endl;
  Timer timer;
  timer.reset();
  m_frame = m_window_local.provider->getFrameBackground(
      m_window_width, m_window_height, x, y, z);
  m_mask = Mat(m_frame.rows, m_frame.cols, CV_8UC1, 0);
  // cout << "Frame Time: " << timer.getTimeUs() / 1000 << " ms" << endl;
  m_has_frame = true;
  #endif
}
void WaiterServer::updateFrameForeground() {
  #if 0
  double x, y, z;
  m_window_controller->getXYZ(x, y, z);
  m_need_update_foreground = m_window_local.provider->hasFrameForeground(
      m_window_width, m_window_height, x, y, z);
  // cout << "m_need_update_foreground: " << m_need_update_foreground << endl;
  if (m_need_update_foreground) {
    cv::Mat foregournd_frame;
    cv::Mat foregournd_mask;
    m_window_local.provider->getFrameForeground(m_window_width, m_window_height,
                                                x, y, z, foregournd_frame,
                                                foregournd_mask);
    // imshow("foregournd_frame", foregournd_frame);
    // imshow("foregournd_mask", foregournd_mask);
    foregournd_frame.copyTo(m_frame, foregournd_mask);
    m_mask = foregournd_mask;
  }
  m_has_frame = true;
  #endif
}

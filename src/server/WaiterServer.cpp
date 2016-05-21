#include "WaiterServer.hpp"

#include "FrameProvider.hpp"
#include "WindowController.hpp"
#include "Timer.hpp"

using namespace std;
using namespace cv;

WaiterServer::WaiterServer(std::string path, int w, int h, int video_mode) {
  m_path = path;
  m_w = w;
  m_h = h;
  m_frame_provider = make_shared<FrameProvider>(m_path, video_mode);

  int n_layers = m_frame_provider->getNumLayers();
  Size top_layer_size(m_frame_provider->getLayerWidth(n_layers - 1),
                      m_frame_provider->getLayerHeight(n_layers - 1));
  Size winsize(w, h);
  m_window_controller =
      make_shared<WindowController>(n_layers, top_layer_size, winsize);

  updateFrameBackground();
  updateFrameForeground();
  m_has_frame = true;
  m_has_thumbnail = false;
  m_need_update_foreground = false;
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
  if (m_need_update_foreground) {
    updateFrameForeground();
    m_has_frame = true;
  }
  return m_has_frame;
}
void WaiterServer::getFrame(cv::Mat &frame) {
  // getFrame
  frame = m_frame;
  m_has_frame = false;
}
bool WaiterServer::hasThumbnail() {
  // hasThumbnail
  return m_has_thumbnail;
}
void WaiterServer::getThumbnail(std::vector<cv::Mat> &thumbnail) {
  // getThumbnail
  thumbnail = m_thumbnail;
  m_has_thumbnail = false;
}
void WaiterServer::updateFrameBackground() {
  double x, y, z;
  m_window_controller->getXYZ(x, y, z);
  //   cout << "x: " << x << "\ty: " << y << "\tz: " << z << endl;
  Timer timer;
  timer.reset();
  m_frame = m_frame_provider->getFrameBackground(m_w, m_h, x, y, z);
  cout << "Frame Time: " << timer.getTimeUs() / 1000 << " ms" << endl;
}
void WaiterServer::updateFrameForeground() {
  double x, y, z;
  m_window_controller->getXYZ(x, y, z);
  m_need_update_foreground =
      m_frame_provider->hasFrameForeground(m_w, m_h, x, y, z);
  cout << "m_need_update_foreground: " << m_need_update_foreground << endl;
  if (m_need_update_foreground) {
    cv::Mat foregournd_frame;
    cv::Mat foregournd_mask;
    m_frame_provider->getFrameForeground(m_w, m_h, x, y, z, foregournd_frame,
                                         foregournd_mask);
    // imshow("foregournd_frame", foregournd_frame);
    // imshow("foregournd_mask", foregournd_mask);
    foregournd_frame.copyTo(m_frame, foregournd_mask);
  }
}

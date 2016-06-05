#include "WaiterServer.hpp"

#include "FrameProvider.hpp"
#include "WindowController.hpp"
#include "Timer.hpp"
#include "WindowProviderLocal.hpp"
#include "Transmitter.hpp"

using namespace std;
using namespace cv;

static void work_listen(int server_id) {
  while (1) {
    cout << "work_listen: listening..." << endl;
    int client_id;
    string client_info;
    if (Transmitter::getClientId(server_id, client_id, client_info)) {
      cout << "work_listen: " << client_info << endl;
    }
  }
}

WaiterServer::WaiterServer(std::string path, cv::Size window_size,
                           int video_mode, int port) {
  m_window_size = window_size;
  {
    FrameProvider provider(path, 0);
    int n_layers = provider.getNumLayers();
    Size top_layer_size(provider.getLayerWidth(n_layers - 1),
                        provider.getLayerHeight(n_layers - 1));
    m_window_controller =
        make_shared<WindowController>(n_layers, top_layer_size, window_size);
  }
  m_window_provider.resize(1);
  m_window_provider[0] =
      make_shared<WindowProviderLocal>(path, video_mode, window_size);
  m_has_frame = false;
  updateFrame();

  if (port > 0) {
    int server_id = Transmitter::initSocketServer(port);
    m_thread_listen = thread(work_listen, server_id);
  }

  cout << "WaiterServer init ok" << endl;
}
void WaiterServer::move(float dx, float dy) {
  m_window_controller->move(dx, dy);
  updateWindowPosition();
  updateFrame();
  m_has_frame = true;
}
void WaiterServer::zoom(float dz) {
  m_window_controller->zoom(dz);
  updateWindowPosition();
  updateFrame();
  m_has_frame = true;
}
bool WaiterServer::hasFrame() {
  if (!m_auto_path.empty()) {
    m_window_controller->setPosition(m_auto_path[m_auto_index]);
    updateWindowPosition();
    m_auto_index++;
    if (m_auto_index == (int)m_auto_path.size()) {
      m_auto_path.clear();
    }
  }
  updateFrame();
  m_has_frame = true;
  return m_has_frame;
}
void WaiterServer::getFrame(cv::Mat &frame) {
  frame = m_frame;
  m_has_frame = false;
}
bool WaiterServer::hasThumbnail() {
  //
  return true;
}
void WaiterServer::getThumbnail(std::vector<cv::Mat> &thumbnail) {
  thumbnail.clear();
  for (size_t i = 0; i < m_window_provider.size(); ++i) {
    vector<Mat> tmp;
    vector<Point3d> tmp_pos;
    m_window_provider[i]->getThumbnail(tmp, tmp_pos);
    thumbnail.insert(thumbnail.end(), tmp.begin(), tmp.end());
  }
}
void WaiterServer::getLinearPath(cv::Point3d position_src,
                                 cv::Point3d position_dst, int len,
                                 std::vector<cv::Point3d> &path) {
  path.resize(len);
  for (int i = 0; i < len; ++i) {
    path[i].x = position_src.x + (position_dst.x - position_src.x) / len * i;
    path[i].y = position_src.y + (position_dst.y - position_src.y) / len * i;
    path[i].z = position_src.z + (position_dst.z - position_src.z) / len * i;
  }
}
void WaiterServer::setThumbnailIndex(int index) {
  vector<Point3d> camera_position;
  getDesinationPosition(camera_position);
  if (index < 0 || index >= (int)camera_position.size()) {
    cout << "index error: " << index << endl;
    return;
  }
  m_auto_path.clear();
  m_auto_index = 0;

  Point3d position_src;
  m_window_controller->getPosition(position_src);
  Point3d position_dst = camera_position[index];
  double mid_z = 0;
  Point3d position_mid1(position_src.x, position_src.y, mid_z);
  Point3d position_mid2(position_dst.x, position_dst.y, mid_z);

  std::vector<cv::Point3d> path;
  getLinearPath(position_src, position_mid1, 15, path);
  m_auto_path.insert(m_auto_path.end(), path.begin(), path.end());
  getLinearPath(position_mid1, position_mid2, 20, path);
  m_auto_path.insert(m_auto_path.end(), path.begin(), path.end());
  getLinearPath(position_mid2, position_dst, 15, path);
  m_auto_path.insert(m_auto_path.end(), path.begin(), path.end());
  m_auto_path.push_back(position_dst);

  // for(size_t i=0; i<m_auto_x.size(); ++i){
  //   cout << i << ": " << m_auto_path[i] << endl;
  // }
  // exit(-1);
}
void WaiterServer::updateWindowPosition() {
  Point3d postion;
  m_window_controller->getPosition(postion);
  for (size_t i = 0; i < m_window_provider.size(); ++i) {
    m_window_provider[i]->setWindowPosition(postion);
  }
}
void WaiterServer::updateFrame() {
  if (m_window_provider.empty()) {
    cout << "error:m_window_provider emtpy" << endl;
    return;
  }
  m_has_frame = false;
  for (size_t i = 0; i < m_window_provider.size(); ++i) {
    if (m_window_provider[i]->hasFrame()) {
      m_has_frame = true;
    }
  }
  if (m_has_frame) {
    Mat frame, mask;
    m_window_provider[0]->getFrame(frame, mask);
    for (size_t i = 1; i < m_window_provider.size(); ++i) {
      Mat tmp_frame;
      Mat tmp_mask;
      m_window_provider[i]->getFrame(tmp_frame, tmp_mask);
      tmp_frame.copyTo(frame, tmp_mask);
    }
    m_frame = frame;
  }
}
void WaiterServer::getDesinationPosition(std::vector<cv::Point3d> &position) {
  position.clear();
  for (size_t i = 0; i < m_window_provider.size(); ++i) {
    vector<Mat> thumbnail;
    vector<Point3d> tmp;
    m_window_provider[i]->getThumbnail(thumbnail, tmp);
    position.insert(position.end(), tmp.begin(), tmp.end());
  }
}

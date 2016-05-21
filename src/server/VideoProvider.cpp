#include "VideoProvider.hpp"

#include <cassert>
#include <iostream>
using namespace std;
using namespace cv;

#include "DirDealer.hpp"
#include "PyCameraSetImage.hpp"
#include "IO.hpp"

const int NUM_LAYERS = 5;

static std::vector<cv::Point2f> getCornerOnFrame(cv::Size size) {
  int rows = size.height;
  int cols = size.width;
  vector<Point2f> corner_frame(4);
  corner_frame[0] = Point2f(0, 0);
  corner_frame[1] = Point2f(cols, 0);
  corner_frame[2] = Point2f(cols, rows);
  corner_frame[3] = Point2f(0, rows);

  return corner_frame;
}
static std::vector<cv::Point2f> getCornerOnScene(cv::Size size, cv::Mat H) {
  vector<Point2f> corner_frame = getCornerOnFrame(size);

  vector<Point2f> corner_on_scene;
  perspectiveTransform(corner_frame, corner_on_scene, H);
  return corner_on_scene;
}
VideoProvider::VideoProvider(string path, bool online) {
  m_path = path;
  vector<string> list;
  {
    ifstream fin;
    assert(IO::openIStream(fin, path + "list.txt", "list load"));
    assert(IO::loadStringList(fin, list));
  }

  if (online) {
    // m_camera_set =
    // make_shared<CameraSetParallel>(make_shared<CameraSetFly2>());
    // m_camera_set = make_shared<CameraSetFly2>();
  } else {
    m_camera_set = make_shared<PyCameraSetImage>(path);
  }
  m_trans.resize(NUM_LAYERS);
  m_rect.resize(NUM_LAYERS);
  for (int layer_id = 0; layer_id < NUM_LAYERS; ++layer_id) {
    int n_cameras = m_camera_set->getNumCamera();
    m_trans[layer_id].resize(n_cameras);
    m_rect[layer_id].resize(n_cameras);
    for (int camera_id = 0; camera_id < n_cameras; ++camera_id) {
      ifstream fin;
      assert(IO::openIStream(fin, path + list[camera_id] + "/info_" +
                                      to_string(layer_id) + ".txt",
                             "VideoData load"));
      assert(IO::loadTransMat(fin, m_trans[layer_id][camera_id]));
      assert(IO::loadRect(fin, m_rect[layer_id][camera_id]));
    }
  }
}
int VideoProvider::getNumCamera() {
  //
  return m_camera_set->getNumCamera();
}
bool VideoProvider::getRectOnScene(cv::Rect &rect, int layer_id,
                                   int camera_id) {
  if (!isValidLayer(layer_id))
    return false;
  if (!isValidCamera(camera_id))
    return false;
  rect = m_rect[layer_id][camera_id];
  return true;
}
bool VideoProvider::getFrame(cv::Mat &frame, int layer_id, int camera_id) {
  if (!isValidLayer(layer_id))
    return false;
  if (!isValidCamera(camera_id))
    return false;
  if (!m_camera_set->read(frame, camera_id, layer_id)) {
    return false;
  }
  Mat show;
  resize(frame, show, Size(frame.cols / 8, frame.rows / 8));
  // imshow("video", show);
  // cout << H << endl;
  Mat dst(m_rect[layer_id][camera_id].height, m_rect[layer_id][camera_id].width,
          CV_8UC3);
  warpPerspective(frame, dst, m_trans[layer_id][camera_id], dst.size());

  resize(dst, show, Size(frame.cols / 8, frame.rows / 8));
  // imshow("video-warp", show);

  // TODO: remove black edge
  std::vector<cv::Point2f> corner_frame = getCornerOnFrame(frame.size());
  std::vector<cv::Point2f> corner_scene =
      getCornerOnScene(frame.size(), m_trans[layer_id][camera_id]);
  line(dst, corner_scene[0], corner_scene[1], Scalar(0, 0, 0), 20);
  line(dst, corner_scene[1], corner_scene[2], Scalar(0, 0, 0), 20);
  line(dst, corner_scene[2], corner_scene[3], Scalar(0, 0, 0), 20);
  line(dst, corner_scene[3], corner_scene[0], Scalar(0, 0, 0), 20);

  // cout << dst.size() << endl;
  frame = dst;
  return true;
}
bool VideoProvider::isValidLayer(int layer_id) {
  if (layer_id < 0 || layer_id >= NUM_LAYERS) {
    return false;
  }
  return true;
}
bool VideoProvider::isValidCamera(int camera_id) {
  if (camera_id < 0 || camera_id >= m_camera_set->getNumCamera()) {
    return false;
  }
  return true;
}
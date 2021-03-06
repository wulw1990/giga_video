#include "VideoProvider.hpp"

#include <cassert>
#include <iostream>
using namespace std;
using namespace cv;

#include "DirDealer.hpp"
#include "PyCameraSetImage.hpp"
#include "PyCameraSetFly2.hpp"
#include "IO.hpp"

VideoProvider::VideoProvider(string path, bool online) {
  cout << "VideoProvider::init begin" << endl;

  {
    ifstream fin;
    assert(IO::openIStream(fin, path + "n_layers.txt", "VideoData load"));
    fin >> NUM_LAYERS;
    cout << "VideoProvider::init NUM_LAYERS=" << NUM_LAYERS << endl;
  }

  if (online) {
    m_camera_set = make_shared<PyCameraSetFly2>(NUM_LAYERS);
  } else {
    m_camera_set = make_shared<PyCameraSetImage>(path);
  }

  m_trans.resize(NUM_LAYERS);
  m_rect.resize(NUM_LAYERS);
  for (int layer_id = 0; layer_id < NUM_LAYERS; ++layer_id) {
    // cout << "VideoProvider::init layer=" << layer_id << endl;
    int n_cameras = m_camera_set->getNumCamera();
    m_trans[layer_id].resize(n_cameras);
    m_rect[layer_id].resize(n_cameras);
    for (int camera_id = 0; camera_id < n_cameras; ++camera_id) {
      ifstream fin;
      string name_camera;
      m_camera_set->getCameraName(name_camera, camera_id);
      string full_name_info =
          path + name_camera + "/info_" + to_string(layer_id) + ".txt";
      assert(IO::openIStream(fin, full_name_info, "VideoData load"));
      assert(IO::loadTransMat(fin, m_trans[layer_id][camera_id]));
      assert(IO::loadRect(fin, m_rect[layer_id][camera_id]));
    }
  }
  cout << "VideoProvider::init end" << endl;
}
int VideoProvider::getNumCamera() {
  //
  return m_camera_set->getNumCamera();
}
bool VideoProvider::getRect(cv::Rect &rect, int camera_id, int layer_id) {
  if (!isValidLayer(layer_id)) {
    cout << "invalid layer_id: " << layer_id << endl;
    return false;
  }
  if (!isValidCamera(camera_id)) {
    cout << "invalid camera_id: " << camera_id << endl;
    return false;
  }
  rect = m_rect[layer_id][camera_id];
  return true;
}
bool VideoProvider::getFrame(cv::Mat &frame, cv::Mat &mask, int camera_id,
                             int layer_id) {
  // cout << "VideoProvider::getFrame begin" << endl;
  if (!isValidLayer(layer_id))
    return false;
  if (!isValidCamera(camera_id))
    return false;

  Mat raw_frame;
  if (!m_camera_set->read(raw_frame, camera_id, layer_id)) {
    return false;
  }
  // imshow("raw_frame", raw_frame);
  // waitKey(0);
  Mat raw_mask(raw_frame.size(), CV_8UC1, Scalar(255));
  Size frame_size;
  {
    frame_size.width = m_rect[layer_id][camera_id].width;
    frame_size.height = m_rect[layer_id][camera_id].height;
  }
  warpPerspective(raw_frame, frame, m_trans[layer_id][camera_id], frame_size);
  warpPerspective(raw_mask, mask, m_trans[layer_id][camera_id], frame_size);
  threshold(mask, mask, 250, 255, THRESH_BINARY);
  // imshow("mask", mask);
  // cout << "VideoProvider::getFrame end" << endl;
  return true;
}
bool VideoProvider::getThumbnail(cv::Mat &thumbnail, int camera_id) {
  //
  if (!isValidCamera(camera_id))
    return false;
  int layer_id = 2;
  m_camera_set->read(thumbnail, camera_id, layer_id);
  return !thumbnail.empty();
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
#include "PyCameraSetFly2.hpp"

#include "IO.hpp"
#include "DirDealer.hpp"
#include "CameraSetFly2.hpp"

using namespace std;
using namespace cv;

PyCameraSetFly2::PyCameraSetFly2(int n_layers) {
  //
  m_n_layers = n_layers;
  m_camera_set = make_shared<CameraSetFly2>();
}

int PyCameraSetFly2::getNumCamera() {
  //
  return m_camera_set->getNumCamera();
}
int PyCameraSetFly2::getNumLayer() {
  //
  return m_n_layers;
}
bool PyCameraSetFly2::read(cv::Mat &frame, int camera_id, int layer_id) {
  //
  if (camera_id < 0 || camera_id >= getNumCamera()) {
    cerr << "invalid camera_id" << endl;
    return false;
  }
  if (layer_id < 0 || layer_id >= getNumLayer()) {
    cerr << "invalid layer_id" << endl;
    return false;
  }
  if(!m_camera_set->read(frame, camera_id)){
      return false;
  }
  if(frame.empty()){
      return false;
  }
  // cout << "PyCameraSetFly2::read diff layer=" << m_n_layers - 1 - layer_id << endl;
  float scale = std::pow(2, m_n_layers - 1 - layer_id);
  // cout << "PyCameraSetFly2::read scale: " << scale << endl;
  resize(frame, frame, Size(frame.cols/scale, frame.rows/scale));
  return true;
}
bool PyCameraSetFly2::getCameraName(std::string &name, int camera_id) {
  if (camera_id < 0 || camera_id >= getNumCamera()) {
    cerr << "invalid camera_id" << endl;
    return false;
  }
  name = to_string(camera_id);
  return true;
}

#include "PyCameraSetImage.hpp"

#include "IO.hpp"
#include "DirDealer.hpp"

using namespace std;
using namespace cv;

PyCameraSetImage::PyCameraSetImage(std::string path) {
  //
  m_path = path;

  // read camera name list: m_camera_name
  {
    ifstream fin;
    assert(IO::openIStream(fin, path + "list.txt", "list load"));
    assert(IO::loadStringList(fin, m_camera_name));
  }

  // m_layer_name: hard code
  m_layer_name.push_back("video_0");
  m_layer_name.push_back("video_1");
  m_layer_name.push_back("video_2");
  m_layer_name.push_back("video_3");
  m_layer_name.push_back("video_4");

  // get frame name list of each camera
  for (size_t camera_id = 0; camera_id < m_camera_name.size(); ++camera_id) {
    cout << "camera: " << m_camera_name[camera_id] << endl;
    for (size_t layer_id = 0; layer_id < m_camera_name.size(); ++layer_id) {
      vector<string> list = DirDealer::getFileList(
          m_path + m_camera_name[camera_id] + "/" + m_layer_name[layer_id]);
          
    }
  }
}

int PyCameraSetImage::getNumCameras() {
  //
  return (int)m_camera_name.size();
}
int PyCameraSetImage::getNumLayers() {
  //
  return (int)m_layer_name.size();
}
bool PyCameraSetImage::read(cv::Mat &frame, int camera_id, int layer_id) {
  //
  if (camera_id < 0 || camera_id >= getNumCameras()) {
    cerr << "invalid camera_id" << endl;
    return false;
  }
  if (layer_id < 0 || layer_id >= getNumLayers()) {
    cerr << "invalid layer_id" << endl;
    return false;
  }
  return true;
}
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
  m_camera_frame_name.resize(m_camera_name.size());
  for (size_t camera_id = 0; camera_id < m_camera_name.size(); ++camera_id) {
    // cout << "camera: " << m_camera_name[camera_id] << endl;
    for (size_t layer_id = 0; layer_id < m_layer_name.size(); ++layer_id) {
      vector<string> list = DirDealer::getNameListValidImage(
          m_path + m_camera_name[camera_id] + "/" + m_layer_name[layer_id]);
      assert(!list.empty());
      if (m_camera_frame_name[camera_id].empty()) {
        sort(list.begin(), list.end());
        m_camera_frame_name[camera_id] = list;
      } else {
        assert(m_camera_frame_name[camera_id].size() == list.size());
      }
    } // for layer_id

    // cout << m_camera_frame_name[camera_id].size() << endl;

  } // for camera_id

  //   exit(-1);

  // frame id
  m_frame_id = vector<int>(m_layer_name.size(), 0);
}

int PyCameraSetImage::getNumCamera() {
  //
  return (int)m_camera_name.size();
}
int PyCameraSetImage::getNumLayer() {
  //
  return (int)m_layer_name.size();
}
bool PyCameraSetImage::read(cv::Mat &frame, int camera_id, int layer_id) {
  //
  if (camera_id < 0 || camera_id >= getNumCamera()) {
    cerr << "invalid camera_id" << endl;
    return false;
  }
  if (layer_id < 0 || layer_id >= getNumLayer()) {
    cerr << "invalid layer_id" << endl;
    return false;
  }
  int frame_id = m_frame_id[camera_id];
  string name = m_path + m_camera_name[camera_id] + "/" +
                m_layer_name[layer_id] + "/" +
                m_camera_frame_name[camera_id][frame_id];
  //   cout << "name : " << name << endl;
  frame = imread(name);
  m_frame_id[camera_id]++;
  if (m_frame_id[camera_id] == (int)m_camera_frame_name[camera_id].size()) {
    m_frame_id[camera_id] = 0;
  }
  return !frame.empty();
}
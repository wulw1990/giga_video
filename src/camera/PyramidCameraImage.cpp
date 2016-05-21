#include "PyramidCameraImage.hpp"

#include "DirDealer.hpp"
#include "IO.hpp"

using namespace cv;
using namespace std;

const string SUFFIX = ".jpg";

PyramidCameraImage::PyramidCameraImage(std::string path) {
  //
  m_path = path;
  m_layer_name.push_back("video_0");
  m_layer_name.push_back("video_1");
  m_layer_name.push_back("video_2");
  m_layer_name.push_back("video_3");
  m_layer_name.push_back("video_4");

  m_frame_name.resize(m_layer_name.size());

  for (size_t i = 0; i < m_layer_name.size(); ++i) {
    vector<string> list = DirDealer::getNameListValidImage(path + m_layer_name[i]);
    sort(list.begin(), list.end());
    m_frame_name[i] = list;
    m_frame_id = 0;
  }

  for (size_t i = 1; i < m_layer_name.size(); ++i) {
    assert(m_frame_name[0].size() == m_frame_name[1].size());
  }
}

bool PyramidCameraImage::read(cv::Mat &frame, int layer_id) {
  //
  if (layer_id < 0 && layer_id >= getNumLayers())
    return false;
  frame = imread(m_path + m_layer_name[layer_id] + "/" +
                 m_frame_name[layer_id][m_frame_id]);
  m_frame_id++;
  if(m_frame_id==(int)m_frame_name[0].size()){
      m_frame_id = 0;
  }
  return true;
}
int PyramidCameraImage::getNumLayers() {
  //
  return (int)m_layer_name.size();
}
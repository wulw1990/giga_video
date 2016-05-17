#include "CameraSetImage.hpp"

using namespace std;
using namespace cv;

#include "DirDealer.h"

const string SUFFIX = ".jpg";

CameraSetImage::CameraSetImage(const std::vector<std::string>& video_name){
	m_video_name = video_name;
	m_frame_num.resize(video_name.size());
	m_frame_id.resize(video_name.size());

	DirDealer dir_dealer;
	for(size_t i=0; i<m_frame_num.size(); ++i){
		vector<string> list =  dir_dealer.getFileList(video_name[i]);
		m_frame_num[i] = (int)list.size();
		// cout << "m_frame_num No." << i << " : " << m_frame_num[i] << endl;
		m_frame_id[i] = 0;
	}

}
bool CameraSetImage::read(cv::Mat& frame, int index){
	if(index>=getNumCamera() || index<0){
		return false;
	}
	frame = imread(m_video_name[index] + "/" + to_string(m_frame_id[index]) + SUFFIX);
	m_frame_id[index]++;
	if(m_frame_id[index]==m_frame_num[index]){
		m_frame_id[index] = 0;
	}
	return !frame.empty();
}
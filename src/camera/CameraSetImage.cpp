#include "CameraSetImage.hpp"

using namespace std;
using namespace cv;

#include "DirDealer.hpp"

const string SUFFIX = ".jpg";

CameraSetImage::CameraSetImage(const std::vector<std::string>& video_name){
	m_video_name = video_name;
	m_frame_name.resize(video_name.size());
	m_frame_id.resize(video_name.size());

	DirDealer dir_dealer;
	for(size_t i=0; i<video_name.size(); ++i){
		vector<string> list =  dir_dealer.getFileList(video_name[i]);
		vector<string> list_valid;
		for(size_t j=0; j<list.size(); ++j){
			string name = list[j];
			string suffix = name.substr(name.length()-4, 4);
			if(suffix==SUFFIX){
				list_valid.push_back(name);
			}
		}
		sort(list_valid.begin(), list_valid.end());
		m_frame_name[i] = list_valid;
		// cout << m_frame_name[i].size() << endl;
		// cout << "m_frame_num No." << i << " : " << m_frame_num[i] << endl;
		m_frame_id[i] = 0;
	}

}
bool CameraSetImage::read(cv::Mat& frame, int index){
	if(index>=getNumCamera() || index<0){
		return false;
	}
	int id = m_frame_id[index];
	frame = imread(m_video_name[index] + "/" + m_frame_name[index][id]);
	m_frame_id[index]++;
	if(m_frame_id[index]==(int)m_frame_name[index].size()){
		m_frame_id[index] = 0;
	}
	return !frame.empty();
}
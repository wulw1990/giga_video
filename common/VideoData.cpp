#include "VideoData.hpp"

#include <cassert>
#include <iostream>
using namespace std;
using namespace cv;

#include "DirDealer.h"
#include "CameraBase.hpp"
#include "CameraVirtual.hpp"
#include "IO.hpp"

VideoData::VideoData(std::string path): NAME_VIDEO("video.avi"), NAME_INFO("info.txt") {
	this->path = path;
}
void VideoData::load() {
	ifstream fin;
	assert(IO::openIStream(fin, path + NAME_INFO, "VideoData load"));
	assert(IO::loadTransMat(fin, H));
	assert(IO::loadRect(fin, rect_on_scene));
}
void VideoData::save(std::string video_src, cv::Mat H, cv::Rect rect_on_scene) {
	this->H = H.clone();
	this->rect_on_scene = rect_on_scene;

	string cmd = "cp " + video_src + " " + path + NAME_VIDEO;
	system(cmd.c_str());

	std::ofstream fout;
	assert(IO::openOStream(fout, path + NAME_INFO, "VideoData save"));
	assert(IO::saveTransMat(fout, H));
	assert(IO::saveRect(fout, rect_on_scene));
}
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
cv::Mat VideoData::getFrame() {
	if (!capture.isOpened()) {
		openCapture();
	}
	cv::Mat frame;
	if (!capture.read(frame)) {
		capture.release();
		openCapture();
		capture >> frame;
	}

	// cout << H << endl;
	Mat dst(rect_on_scene.height, rect_on_scene.width, CV_8UC3);
	warpPerspective(frame, dst, H, dst.size());


	// TODO: remove black edge
	// std::vector<cv::Point2f> corner_frame = getCornerOnFrame(frame.size());
	std::vector<cv::Point2f> corner_scene = getCornerOnScene(frame.size(), H);
	line(dst, corner_scene[0], corner_scene[1], Scalar(255, 0, 0), 3);
	line(dst, corner_scene[1], corner_scene[2], Scalar(255, 0, 0), 3);
	line(dst, corner_scene[2], corner_scene[3], Scalar(255, 0, 0), 3);
	line(dst, corner_scene[3], corner_scene[0], Scalar(255, 0, 0), 3);

	// cout << dst.size() << endl;

	return dst;
}
void VideoData::openCapture() {
	capture.open(path + NAME_VIDEO);
	if (!capture.isOpened()) {
		cerr << "can not open video: " << path + NAME_VIDEO;
		exit(-1);
	}
}
MultiVideoData::MultiVideoData(std::string path): NAME_LIST("list.txt") {
	this->path = path;
	ifstream fin( (path + NAME_LIST).c_str() );
	if (!fin.is_open()) {
		cerr << "MultiVideoData::MultiVideoData: can not open: " << path + NAME_LIST << endl;
		exit(-1);
	}
	string str;
	video_data.clear();
	while (fin >> str) {
		video_data.push_back( VideoData(path + str + "/") );
	}
	for (size_t i = 0; i < video_data.size(); ++i) {
		video_data[i].load();
	}
	cout << "video_data: " << video_data.size() << endl;
}


CameraSetData::CameraSetData(string path) {
	m_path = path;

	string video_path = path + "data/";
	vector<string> video_name;
	video_name.push_back(video_path + "0.avi");
	video_name.push_back(video_path + "1.avi");

	m_camera_set = make_shared<CameraVirtual>(video_name);

	int n_cameras = m_camera_set->getNumCamera();
	m_trans.resize(n_cameras);
	m_rect.resize(n_cameras);

	for (int i = 0; i < n_cameras; ++i) {
		ifstream fin;
		assert(IO::openIStream(fin, path + "info/" + to_string(i) + ".txt", "VideoData load"));
		assert(IO::loadTransMat(fin, m_trans[i]));
		assert(IO::loadRect(fin, m_rect[i]));
	}
}
int CameraSetData::getNumCamera() {
	return m_camera_set->getNumCamera();
}
bool CameraSetData::getRectOnScene(cv::Rect& rect, int index) {
	if(index<0 || index>= m_camera_set->getNumCamera()){
		return false;
	}
	rect = m_rect[index];
	return true;
}
bool CameraSetData::getFrame(cv::Mat& frame, int index) {
	if(! m_camera_set->read(frame, index) ){
		return false;
	}
	// cout << H << endl;
	Mat dst(m_rect[index].height, m_rect[index].width, CV_8UC3);
	warpPerspective(frame, dst, m_trans[index], dst.size());

	// TODO: remove black edge
	std::vector<cv::Point2f> corner_frame = getCornerOnFrame(frame.size());
	std::vector<cv::Point2f> corner_scene = getCornerOnScene(frame.size(), m_trans[index]);
	line(dst, corner_scene[0], corner_scene[1], Scalar(255, 0, 0), 3);
	line(dst, corner_scene[1], corner_scene[2], Scalar(255, 0, 0), 3);
	line(dst, corner_scene[2], corner_scene[3], Scalar(255, 0, 0), 3);
	line(dst, corner_scene[3], corner_scene[0], Scalar(255, 0, 0), 3);

	// cout << dst.size() << endl;
	frame = dst;
	return true;
}
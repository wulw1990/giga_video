#include "VideoProvider.hpp"

#include <cassert>
#include <iostream>
using namespace std;
using namespace cv;

#include "DirDealer.h"
#include "CameraSetBase.hpp"
#include "CameraSetFly2.hpp"
#include "CameraSetVirtual.hpp"
#include "IO.hpp"

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
VideoProvider::VideoProvider(string path) {
	m_path = path;

#if 0
	string video_path = path + "data/";
	vector<string> video_name;
	video_name.push_back(video_path + "0.avi");
	// video_name.push_back(video_path + "1.avi");
	m_camera_set = make_shared<CameraSetVirtual>(video_name);
#else
	m_camera_set = make_shared<CameraSetFly2>();
#endif
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
int VideoProvider::getNumCamera() {
	return m_camera_set->getNumCamera();
}
bool VideoProvider::getRectOnScene(cv::Rect& rect, int index) {
	if(index<0 || index>= m_camera_set->getNumCamera()){
		return false;
	}
	rect = m_rect[index];
	return true;
}
bool VideoProvider::getFrame(cv::Mat& frame, int index) {
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
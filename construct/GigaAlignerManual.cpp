#include "GigaAlignerManual.hpp"

#include <iostream>
using namespace std;
using namespace cv;

static float getShowImage(cv::Mat img, cv::Mat& show) {
	const int w_max = 1800;
	const int h_max = 900;
	show = img.clone();
	float scale = 1.0f;
	while (show.rows > h_max || show.cols > w_max) {
		resize(show, show, Size(show.cols / 2, show.rows / 2));
		scale *= 2;
	}
	return scale;
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
static cv::Rect getRectFromCorner(std::vector<cv::Point2f>& corner) {
	assert(!corner.empty());
	int x1 = corner[0].x;
	int y1 = corner[0].y;
	int x2 = corner[0].x;
	int y2 = corner[0].y;
	for (size_t i = 0; i < corner.size(); ++i) {
		if (corner[i].x < x1) x1 = corner[i].x;
		if (corner[i].x > x2) x2 = corner[i].x;
		if (corner[i].y < y1) y1 = corner[i].y;
		if (corner[i].y > y2) y2 = corner[i].y;
		// cout << corner[i] << endl;
	}
	return Rect(x1, y1, x2 - x1 + 1, y2 - y1 + 1);
}
bool GigaAlignerManual::alignToWin(cv::Mat frame, cv::Mat win, cv::Mat& trans, cv::Rect& rect) {
	// cout << "align manually..." << endl;

	Mat show_frame;
	Mat show_win;
	float scale_frame = getShowImage(frame, show_frame);
	float scale_win = getShowImage(win, show_win);

	cout << "scale_frame: " << scale_frame << endl;
	cout << "scale_win: " << scale_win << endl;


	while (1) {
		for(size_t i=0; i<m_point_frame.size(); ++i){
			circle(show_frame, m_point_frame[i], 3, Scalar(255, 0, 0), -1);
		}
		for(size_t i=0; i<m_point_win.size(); ++i){
			circle(show_win, m_point_win[i], 3, Scalar(255, 0, 0), -1);
		}

		imshow("frame", show_frame);
		imshow("win", show_win);

		cv::setMouseCallback("frame", onMouseFrame, &m_point_frame);
		cv::setMouseCallback("win", onMouseWin, &m_point_win);

		char key = waitKey(33);
		if(key=='q'){
			break;
		}
	}
	if(m_point_win.size()!=m_point_frame.size()){
		return false;
	}
	for(size_t i=0; i<m_point_frame.size(); ++i){
		m_point_frame[i].x *= scale_frame;
		m_point_frame[i].y *= scale_frame;
		m_point_win[i].x *= scale_win;
		m_point_win[i].y *= scale_win;
	}

	trans = findHomography(m_point_frame, m_point_win);

	vector<Point2f> corner_scene = getCornerOnScene(frame.size(), trans);

	rect = getRectFromCorner(corner_scene);


	return true;
}
void GigaAlignerManual::onMouseFrame(int event, int x, int y, int, void* data)
{
	if (event != EVENT_LBUTTONDOWN)
		return;
	std::vector<Point2f>* point = (std::vector<Point2f>*)data;
	switch (event)
	{
	case EVENT_LBUTTONDOWN:
		cout << "frame x: " << x  << " y: " << y << endl;
		point->push_back(Point2f(x, y));
		break;
	}
}
void GigaAlignerManual::onMouseWin(int event, int x, int y, int, void* data)
{
	if (event != EVENT_LBUTTONDOWN)
		return;
	std::vector<Point2f>* point = (std::vector<Point2f>*)data;
	switch (event)
	{
	case EVENT_LBUTTONDOWN:
		cout << "win x: " << x  << " y: " << y << endl;
		point->push_back(Point2f(x, y));
		break;
	}
}
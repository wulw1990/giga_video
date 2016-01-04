#include "GigaAlignerBase.hpp"

using namespace cv;
using namespace std;
#include "FrameProvider.hpp"
#include "Timer.hpp"

GigaAlignerBase::GigaAlignerBase(string path){
	m_frame_provider = make_shared<FrameProvider>(path, false);
}
static void showImage(string win_name, Mat img) {
	const int w_max = 1800;
	const int h_max = 900;
	Mat show = img.clone();
	while (show.rows > h_max || show.cols > w_max)
		resize(show, show, Size(show.cols / 2, show.rows / 2));
	imshow(win_name, show);
	waitKey(1);
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
bool GigaAlignerBase::align(Mat frame, Mat& H, Rect& rect_on_scene){
	int work_layer_id = m_frame_provider->getNumLayers()-1;
	Size work_layer_size(m_frame_provider->getLayerWidth(work_layer_id), m_frame_provider->getLayerHeight(work_layer_id));

	Rect rect_max(0, 0, work_layer_size.width, work_layer_size.height);

	int win_rows = frame.rows * 4;
	int win_cols = frame.cols * 4;
	int step_row = frame.rows * 2;
	int step_col = frame.cols * 2;

	showImage("frame", frame);

	for (int r = 2; r * step_row < work_layer_size.height; ++r) {
		for (int c = 0; c * step_col < work_layer_size.width; ++c) {
			cout << r << "\t" << c << "\t";
			cout .flush();

			Rect rect(c * step_col, r * step_row, win_cols, win_rows);
			rect = rect & rect_max;

			Timer timer;
			timer.reset();
			Mat win = m_frame_provider->getFrame(rect.width, rect.height, rect.x, rect.y, work_layer_id);
			cout << "read ms : " << timer.getTimeUs() / 1000 << "\t";
			cout.flush();

			showImage("win", win);

			timer.reset();
			bool matched = alignToWin(frame, win, H, rect_on_scene);
			cout << matched  << "\tmatch ms : " << timer.getTimeUs() / 1000 << endl;

			std::vector<cv::Point2f> corner_scene = getCornerOnScene(frame.size(), H);
			for (size_t i = 0; i < corner_scene.size(); ++i) {
				corner_scene[i].x += rect_on_scene.x;
				corner_scene[i].y += rect_on_scene.y;
			}
			line(win, corner_scene[0], corner_scene[1], Scalar(255, 0, 0), 3);
			line(win, corner_scene[1], corner_scene[2], Scalar(255, 0, 0), 3);
			line(win, corner_scene[2], corner_scene[3], Scalar(255, 0, 0), 3);
			line(win, corner_scene[3], corner_scene[0], Scalar(255, 0, 0), 3);
			showImage("win", win);
			char key = waitKey(1);

			if (matched) {
				rect_on_scene.x += c * step_col;
				rect_on_scene.y += r * step_row;
				// waitKey(0);
				return true;
			}
		}
	}
	return false;
}

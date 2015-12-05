#include "GigaAligner.h"

#include <iostream>
#include <fstream>
#include <time.h>
#include "GeometryAligner.h"
#include "OpticAligner.h"
#include "DataProvide.hpp"

const int DOWN_SAMPLE_RATE = 2;
const int MAX_OUTPUT_FRAME = 100;
const double INPUT_RATE = 25;
const double OUTPUT_RATE = INPUT_RATE / DOWN_SAMPLE_RATE;

GigaAligner::GigaAligner()
{
	m_optic_aligner = new OpticAligner;
	m_geometry_aligner = new GeometryAligner;
}
GigaAligner::~GigaAligner()
{
	delete m_optic_aligner;
	delete m_geometry_aligner;
}
bool GigaAligner::alignStaticVideo(string path_scene, string input_video, string output_prefix)
{
	//read first frame
	Mat frame = readFirstFrame(input_video);
	//showImage("first frame", frame);
	cout << "Frame Size : " << frame.size() << endl;

	//align first frame
	Mat aligned_frame;
	Rect aligned_rect;
	Mat T;
	Mat model_hist;

	if (!alignFrameToScene(path_scene, frame, aligned_frame, aligned_rect, T, model_hist))
		return false;

	//save 
	cout << "Saving..." << endl;
	string output_video = output_prefix + ".avi";
	string output_txt = output_prefix + ".txt";

	int len = writeStaticVideo(input_video, output_video, T, model_hist, aligned_frame.size());
	cout << "n_frames saved: " << len << endl;;
	writeStaticInfo(output_txt, len, aligned_rect);
	return true;
}
bool GigaAligner::alignFrameToScene(string path_scene, Mat frame, Mat& dst_frame, Rect& dst_rect, Mat& T, Mat& model_hist)
{
	SceneFrameProvider* rect_getter = new SceneFrameProvider(path_scene, path_scene + "info_scene.txt");

	int work_layer_id = 4;
	int scale = 2;

	// Size work_layer_size = rect_getter->getSceneLayerSizePixel(work_layer_id);
	Size work_layer_size(rect_getter->getLayerWidth(work_layer_id), rect_getter->getLayerHeight(work_layer_id));

	Rect rect_max(0, 0, work_layer_size.width, work_layer_size.height);

	int win_rows = frame.rows * 4;
	int win_cols = frame.cols * 4;
	int step_row = frame.rows * 2;
	int step_col = frame.cols * 2;

	for (int r = 1; r*step_row < work_layer_size.height; ++r){
		for (int c = 0; c*step_col < work_layer_size.width; ++c){
			cout << r << "\t" << c << "\t" << "begin..." << endl;
			clock_t start_time = clock();

			Rect rect(c*step_col, r*step_row, win_cols, win_rows);
			rect = rect & rect_max;

			// Mat win = rect_getter->GetRectMat(rect, work_layer_id);
			Mat win = rect_getter->getFrame(rect.width, rect.height, rect.x, rect.y, work_layer_id);
			
			showImage("win", win);
			char key = waitKey(1);
			// if (key != 'y') continue;

			// bool matched = m_geometry_aligner->align(frame, win, dst_frame, dst_rect, T);
			// cout << r << "\t" << c << "\t" << matched  << "\tms : " << clock()-start_time << endl;

			// if (matched){
			// 	model_hist = win(dst_rect).clone();
			// 	dst_rect.x += c * step_col;
			// 	dst_rect.y += r * step_row;
			// 	dst_rect.x *= scale;
			// 	dst_rect.y *= scale;
			// 	dst_rect.width *= scale;
			// 	dst_rect.height *= scale;
			// 	return true;
			// }
		}
	}
	return false;
}
Mat GigaAligner::readFirstFrame(string name)
{
	Mat frame;
	VideoCapture capture(name);
	assert(capture.isOpened());
	capture >> frame;
	capture.release();
	assert(!frame.empty());
	cout << "GigaAligner::readFirstFrame OK" << endl;
	return frame;
}
int GigaAligner::writeStaticVideo(string name_input, string name_output, Mat T, Mat hist, Size size)
{
	VideoCapture capture(name_input);
	VideoWriter writer(name_output, CV_FOURCC('M', 'J', 'P', 'G'), OUTPUT_RATE, size);
	assert(capture.isOpened());
	assert(writer.isOpened());
	
	Mat frame;
	Mat aligned_frame(size, CV_8UC3);
	int input_frame_count = 0;
	int output_frame_count = 0;
	while (capture.read(frame) && !frame.empty()){
		if ((++input_frame_count) % DOWN_SAMPLE_RATE != 0) continue;

		warpAffine(frame, aligned_frame, T, size);
		aligned_frame = m_optic_aligner->getMatchRGBImg(aligned_frame, hist);
		writer << aligned_frame;

		//cout << "input_frame_count: " << input_frame_count << endl;
		//imshow("frame", frame);
		//waitKey(1);

		if (++output_frame_count >= MAX_OUTPUT_FRAME) break;
	}
	writer.release();
	capture.release();
	return output_frame_count - 1;
}
bool GigaAligner::writeStaticInfo(string name, int n, Rect rect)
{
	ofstream ofs(name);
	assert (ofs.is_open());
	ofs << 0 << endl;//0--static
	ofs << n << endl;
	ofs << rect.x << "\t" << rect.y << "\t" << rect.width << "\t" << rect.height << endl;
	ofs.close();
	return true;
}
bool GigaAligner::writeDynamicInfo(string name, int n, vector<Rect> rect)
{
	ofstream ofs(name);
	assert(ofs.is_open());
	assert((int)rect.size() != 0);
	ofs << 1 << endl;//1--dynamic
	ofs << n << endl;
	for (int i = 0; i < n;  ++i)
		ofs << rect[i].x << "\t" << rect[i].y << "\t" << rect[i].width << "\t" << rect[i].height << endl;
	ofs.close();
	return true;
}
void GigaAligner::showImage(string win_name, Mat img)
{
	const int w_max = 1800;
	const int h_max = 900;
	Mat show = img.clone();
	while (show.rows > h_max || show.cols > w_max)
		resize(show, show, Size(show.cols / 2, show.rows / 2));
	imshow(win_name, show);
	waitKey(1);
}
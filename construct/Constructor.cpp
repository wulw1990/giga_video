#include "Constructor.hpp"

#include <cassert>
#include <map>
#include <iostream>
using namespace std;
using namespace cv;

#include "Data.hpp"
#include "DataProvide.hpp"

Constructor::Constructor() {
}
void Constructor::writeInfoForAutopan(std::string path, int n_layers, std::string name) {
	cout << "constructFromAutopan" << endl;
	
	SceneData scene_data;
	scene_data.loadFromDisk(path);
	scene_data.save(path + name);
	
	cout << "verify..." << endl;
	SceneData scene_data_ver;
	scene_data_ver.load(path + name);
	scene_data_ver.save(path + name);
}

//TODO
#include "GigaAligner.h" 
bool Constructor::constructVideo(std::string path_img, std::string raw_video_name, std::string path_out){
	cout << "path_img: " << path_img << endl;
	cout << "raw_video_name: " << raw_video_name << endl;
	cout << "path_out: " << path_out << endl;

	GigaAligner giga_aligner;
	giga_aligner.alignStaticVideo(path_img, raw_video_name, path_out);

	// SceneFrameProvider frame_provider(path_img, path_img+"info_scene.txt");
	// const int default_layer = 4;
	// Mat background = frame_provider.getFrame(1000, 700, 0, 0, 0);
	// imshow("background", background);
	// waitKey(0);

	return true;
}
void Constructor::cutVideo(std::string name_src, std::string name_dst){
	cout << "cutVideo: " << endl;
	cout << "name_src: " << name_src << endl;
	cout << "name_dst: " << name_dst << endl;

	assert(name_dst.substr(name_dst.length()-3, 3)=="avi");

	Size output_size;

	VideoCapture capture(name_src.c_str());
	assert(capture.isOpened());
	vector<Mat> frame_vec;
	Mat frame;
	const int MAX_FRAMES = 1000;
	cout << "reading video" << endl;
	for(int i=0; capture.read(frame) && i<MAX_FRAMES; ++i){
		output_size = frame.size();
		resize(frame, frame, Size(frame.cols/4, frame.rows/4));
		frame_vec.push_back(frame);
	}
	capture.release();

	cout << "cut video..." << endl;
	int index_head = 0;
	int index_back = 0;
	cout << "index_head: " << index_head << " index_back: " << index_back << endl;
	for(int i=0; i<(int)frame_vec.size(); ++i){
		frame = frame_vec[i];
		const int PAD = frame.rows / 10;
		Mat show(frame.rows + PAD * 2, frame.cols, CV_8UC3, Scalar(255, 0, 0));
		frame.copyTo(show(Rect(0, 0, frame.cols, frame.rows)));
		rectangle(show, Rect(0, frame.rows, frame.cols*i/(int)frame_vec.size(), PAD), Scalar(0, 0, 255), -1);

		if(index_back>index_head){
			int x = frame.cols*index_head/(int)frame_vec.size();
			int w = frame.cols*(index_back-index_head)/(int)frame_vec.size();
			rectangle(show, Rect(x, frame.rows + PAD, w, PAD), Scalar(0, 255, 0), -1);
		}else{
			int x = frame.cols*index_head/(int)frame_vec.size();
			rectangle(show, Rect(x, frame.rows + PAD, 2, PAD), Scalar(0, 255, 0), -1);
		}
		imshow("frame", show);
		char key = waitKey(0);
		if(key == 'b'){
			index_head = i;
			cout << "index_head: " << index_head << " index_back: " << index_back << endl;
		}else if(key=='e'){
			index_back = i;
			cout << "index_head: " << index_head << " index_back: " << index_back << endl;
		}else if(key=='p' && i>0){
			i-=2;
		}else if(key=='r'){
			i = -1;
		}else if(key=='q'){
			break;
		}
	}
	destroyAllWindows();

	cout << "saving..." << endl;
	capture.open(name_src);
	assert(capture.isOpened());
	VideoWriter writer(name_dst, CV_FOURCC('M', 'J', 'P', 'G'), 30, output_size);
	assert(writer.isOpened());

	for(int i=0; capture.read(frame); ++i){
		if(i>=index_head && i<=index_back){
			writer << frame;
		}
	}
	capture.release();
	writer.release();
}

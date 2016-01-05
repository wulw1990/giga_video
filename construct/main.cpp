#include <string>
#include <vector>
#include <cassert>
#include <iostream>
#include <algorithm>

using namespace std;

#include "GeometryAligner.hpp"
#include "DirDealer.h"
#include "CameraSetVideo.hpp"
#include "CameraSetFly2.hpp"
#include "GigaAlignerAuto.hpp"
#include "GigaAlignerManual.hpp"
#include "IO.hpp"
#include "TileProvider.hpp"

int construct_from_autopan(int argc, char** argv);
int cut_video(int argc, char** argv);
int test_geo_align(int argc, char** argv);
// int test_giga_align(int argc, char** argv);

int construct_camera_set(int argc, char** argv);

int main(int argc, char **argv) {
	assert(argc >= 2);
	string mode = string( argv[1] );
	argc--;
	argv++;

	transform(mode.begin(), mode.end(), mode.begin(), ::tolower);

	if (mode == "construct_from_autopan") construct_from_autopan(argc, argv);
	else if (mode == "cut_video") cut_video(argc, argv);
	else if (mode == "test_geo_align") test_geo_align(argc, argv);
	else if (mode == "construct_camera_set") construct_camera_set(argc, argv);
	// else if (mode == "test_giga_align") test_giga_align(argc, argv);
	else {
		cerr << "main mode error : " << mode << endl;
		return -1;
	}
	return 0;
}
int construct_from_autopan(int argc, char** argv) {
	cout << "construct_from_autopan demo" << endl;
	assert(argc >= 4);

	string path = argv[1];
	int n_layers = atoi(argv[2]);
	int head_layers = atoi(argv[3]);
	string name = "info_scene.txt";

	SceneData scene_data;
	scene_data.loadFromDisk(path, n_layers, head_layers);
	scene_data.save(path + name);

	cout << "verify..." << endl;
	SceneData scene_data_ver;
	scene_data_ver.load(path + name);
	scene_data_ver.save(path + name);

	return 0;
}
int cut_video(int argc, char** argv) {
	cout << "cut_video demo" << endl;
	assert(argc >= 3);
	string name_src(argv[1]);
	string name_dst(argv[2]);

	cout << "cutVideo: " << endl;
	cout << "name_src: " << name_src << endl;
	cout << "name_dst: " << name_dst << endl;

	assert(name_dst.substr(name_dst.length() - 3, 3) == "avi");

	Size output_size;

	VideoCapture capture(name_src.c_str());
	assert(capture.isOpened());
	vector<Mat> frame_vec;
	Mat frame;
	const int MAX_FRAMES = 1000;
	cout << "reading video" << endl;
	for (int i = 0; capture.read(frame) && i < MAX_FRAMES; ++i) {
		output_size = frame.size();
		resize(frame, frame, Size(frame.cols / 4, frame.rows / 4));
		frame_vec.push_back(frame);
	}
	capture.release();

	cout << "cut video..." << endl;
	int index_head = 0;
	int index_back = 0;
	cout << "index_head: " << index_head << " index_back: " << index_back << endl;
	for (int i = 0; i < (int)frame_vec.size(); ++i) {
		frame = frame_vec[i];
		const int PAD = frame.rows / 10;
		Mat show(frame.rows + PAD * 2, frame.cols, CV_8UC3, Scalar(255, 0, 0));
		frame.copyTo(show(Rect(0, 0, frame.cols, frame.rows)));
		rectangle(show, Rect(0, frame.rows, frame.cols * i / (int)frame_vec.size(), PAD), Scalar(0, 0, 255), -1);

		if (index_back > index_head) {
			int x = frame.cols * index_head / (int)frame_vec.size();
			int w = frame.cols * (index_back - index_head) / (int)frame_vec.size();
			rectangle(show, Rect(x, frame.rows + PAD, w, PAD), Scalar(0, 255, 0), -1);
		} else {
			int x = frame.cols * index_head / (int)frame_vec.size();
			rectangle(show, Rect(x, frame.rows + PAD, 2, PAD), Scalar(0, 255, 0), -1);
		}
		imshow("frame", show);
		char key = waitKey(0);
		if (key == 'b') {
			index_head = i;
			cout << "index_head: " << index_head << " index_back: " << index_back << endl;
		} else if (key == 'e') {
			index_back = i;
			cout << "index_head: " << index_head << " index_back: " << index_back << endl;
		} else if (key == 'p' && i > 0) {
			i -= 2;
		} else if (key == 'r') {
			i = -1;
		} else if (key == 'q') {
			break;
		}
	}
	destroyAllWindows();

	cout << "saving..." << endl;
	capture.open(name_src);
	assert(capture.isOpened());
	VideoWriter writer(name_dst, CV_FOURCC('M', 'J', 'P', 'G'), 15, output_size);
	assert(writer.isOpened());

	for (int i = 0; capture.read(frame), capture.read(frame); i += 2) {
		if (i >= index_head && i <= index_back) {
			writer << frame;
		}
	}
	capture.release();
	writer.release();
	return 0;
}
int test_geo_align(int argc, char** argv) {
	assert(argc >= 3);
	Mat scene = imread(argv[1]);
	Mat frame = imread(argv[2]);
	cv::Mat H;
	cv::Rect rect_on_scene;

	GeometryAligner aligner;
	bool is_matched = aligner.align(frame, scene, H, rect_on_scene);
	cout << "is_matched: " << is_matched << endl;


	// Mat scene1(rect_on_scene.height, rect_on_scene.width, CV_8UC3);
	// warpPerspective(frame, scene1, H, scene1.size());
	// imwrite("../new_scene.jpg", scene1);


	if (is_matched) {
		Mat scene1(rect_on_scene.height, rect_on_scene.width, CV_8UC3);
		warpPerspective(frame, scene1, H, scene1.size());
		for (int r = 0; r < scene1.rows; ++r) {
			for (int c = 0; c < scene1.cols; ++c) {
				if (scene1.at<Vec3b>(r, c) != Vec3b(0, 0, 0)) {
					scene.at<Vec3b>(r + rect_on_scene.y, c + rect_on_scene.x) = scene1.at<Vec3b>(r, c);
				}
			}
		}
		imwrite("../new_scene.jpg", scene);
	}


	return 0;
}
int construct_camera_set(int argc, char** argv) {
	assert(argc >= 4);
	string path(argv[1]);
	string video_mode(argv[2]);
	string align_mode(argv[3]);

	shared_ptr<CameraSetBase> camera_set;
	shared_ptr<GigaAlignerBase> giga_aligner;

	if (video_mode == "fly2") {
		camera_set = make_shared<CameraSetFly2>();
	} else if (video_mode == "virtual") {
		vector<string> video_name;
		video_name.push_back(path + "video/data/0.avi");
		video_name.push_back(path + "video/data/1.avi");
		video_name.push_back(path + "video/data/2.avi");
		video_name.push_back(path + "video/data/3.avi");
		video_name.push_back(path + "video/data/4.avi");
		// video_name.push_back(path + "video/data/5.avi");
		// video_name.push_back(path + "video/data/6.avi");
		camera_set = make_shared<CameraSetVideo>(video_name);
	}else{
		cerr << "error video mode: " << video_mode << endl;
	}

	if(align_mode=="auto"){
		giga_aligner = make_shared<GigaAlignerAuto>(path);
	}else if(align_mode=="manual"){
		giga_aligner = make_shared<GigaAlignerManual>(path);
	}

	int n_cameras = camera_set->getNumCamera();
	vector<Mat> frame(n_cameras);
	for (int i = 0; i < n_cameras; ++i) {
		assert(camera_set->read(frame[i], i));
		// imshow("frame", frame[i]);
		// waitKey(0);
	}

	// for (int i = 0; i < n_cameras; ++i) {
	// for (int i = 0; i < n_cameras; ++i) {
	for (int i = 4; i < 5; ++i) {
		// for (size_t i = 0; i < 1; ++i) {
		cout << "align No." << i << " ..." << endl;
		Mat trans;
		Rect rect;
		if (giga_aligner->align(frame[i], trans, rect)) {
			std::ofstream fout;
			assert(IO::openOStream(fout, path + "video/" + to_string(i) + ".txt", "VideoData save"));
			assert(IO::saveTransMat(fout, trans));
			assert(IO::saveRect(fout, rect));
		}
	}

	return 0;
}

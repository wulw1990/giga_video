#include <string>
#include <vector>
#include <iostream>
#include <cassert>
#include <algorithm>
#include <opencv2/opencv.hpp>
#include <memory>
using namespace std;
using namespace cv;
#include <time.h>
#include "Timer.hpp"
#include "DirDealer.h"

//#include "SocketServer.hpp"
#include "CameraSetFly2.hpp"
#include "CameraSetVideo.hpp"
#include "CameraSetImage.hpp"
#include "CameraSetParallel.hpp"
#include "CameraSetRecorder.hpp"

int demo(int argc, char** argv);
int record(int argc, char** argv);
int demo_virtual(int argc, char** argv);

int main(int argc, char **argv) {
	assert(argc >= 2);
	string mode = string( argv[1] );
	argc--;
	argv++;

	transform(mode.begin(), mode.end(), mode.begin(), ::tolower);

	if (mode == "demo") demo(argc, argv);
	else if (mode == "record") record(argc, argv);
	else if (mode == "demo_virtual") demo_virtual(argc, argv);
	else {
		cerr << "main mode error : " << mode << endl;
		return -1;
	}
	return 0;
}
// Mat mergeFrame(vector<Mat>& frame){
//             	for(size_t i=0; i<frame.size(); ++i){
//             		// resize(frame[i], frame[i], Size(frame[i].cols/scale, frame[i].rows/scale));
//             	}
// 	return frame[0];
// }
int demo(int argc, char** argv) {
	assert(argc >= 2);
	int scale = atoi(argv[1]);

	shared_ptr<CameraSetBase> camera_set;
	if (1) {
		camera_set = make_shared<CameraSetParallel>();
		// camera_set = make_shared<CameraSetFly2>();
	} else {
		string path = "/media/wuliwei/data/NB_BBNC/giga_video/zijing16/video/";
		vector<string> name;
		name.push_back(path + "MVI_6878/video.avi");
		name.push_back(path + "MVI_6880/video.avi");
		name.push_back(path + "MVI_6881/video.avi");
		name.push_back(path + "MVI_6883/video.avi");
		camera_set = make_shared<CameraSetVideo>(name);
	}

	int n_cameras = camera_set->getNumCamera();
	cout << "cameras: " << n_cameras << endl;

	Timer timer;
	while (1) {
		timer.reset();
		Mat frame;
		for (int i = 0; i < n_cameras; ++i) {
			assert(camera_set->read(frame, i));
			// cout << frame.size() << endl;
			resize(frame, frame, Size(frame.cols / scale, frame.rows / scale));
			imshow("frame" + to_string(i), frame);
			waitKey(1);
		}
		// cout << timer.getTimeUs()/1000 << endl;
		// waitKey(33);
	}
	return 0;
}
int record(int argc, char** argv) {
	assert(argc >= 2);
	string path(argv[1]);

	CameraSetRecorder recorder( make_shared<CameraSetParallel>() );
	recorder.record(path);

	return 0;
}
int demo_virtual(int argc, char** argv) {
	const int argc_head = 4;
	assert(argc > argc_head);
	string mode(argv[1]);
	int scale = atoi(argv[2]);
	string path(argv[3]);
	int n_videos = atoi(argv[4]);
	assert(argc > argc_head + n_videos);

	cout << "video mode: " << mode << endl;
	vector<string> name(n_videos);
	for(int i=0; i<n_videos; ++i){
		name[i] = path + string(argv[argc_head+1+i]);
		cout << "video No." << i << " : " << name[i] << endl;
	}	

	shared_ptr<CameraSetBase> camera_set;
	if(mode=="video"){
		camera_set = make_shared<CameraSetParallel>(make_shared<CameraSetVideo>(name));
	}else if(mode=="image"){
		camera_set = make_shared<CameraSetParallel>(make_shared<CameraSetImage>(name));
	}else{
		cerr << "video mode error : " << mode << endl;
		return -1;
	}
	// camera_set = make_shared<CameraSetVideo>(name);

	int n_cameras = camera_set->getNumCamera();
	cout << "cameras: " << n_cameras << endl;

	Timer timer;
	while (1) {
		timer.reset();
		Mat frame;
		for (int i = 0; i < n_cameras; ++i) {
			assert(camera_set->read(frame, i));
			resize(frame, frame, Size(frame.cols / scale, frame.rows / scale));
			imshow("frame" + to_string(i), frame);
			waitKey(1);
		}
		cout << timer.getTimeUs()/1000 << endl;
	}
	return 0;
}
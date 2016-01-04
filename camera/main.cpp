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

//#include "SocketServer.hpp"
#include "CameraSetFly2.hpp"
#include "CameraSetVirtual.hpp"

int demo(int argc, char** argv);
int record(int argc, char** argv);

int main(int argc, char **argv) {
	assert(argc >= 2);
	string mode = string( argv[1] );
	argc--;
	argv++;

	transform(mode.begin(), mode.end(), mode.begin(), ::tolower);

	if (mode == "demo") demo(argc, argv);
	else if (mode == "record") record(argc, argv);
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
	// CameraSet camera_set;
	assert(argc >= 2);
	int scale = atoi(argv[1]);

	shared_ptr<CameraSetBase> camera_set;
	if (1) {
		camera_set = make_shared<CameraSetFly2>();
	} else {
		string path = "/media/wuliwei/data/NB_BBNC/giga_video/zijing16/video/";
		vector<string> name;
		name.push_back(path + "MVI_6878/video.avi");
		name.push_back(path + "MVI_6880/video.avi");
		name.push_back(path + "MVI_6881/video.avi");
		name.push_back(path + "MVI_6883/video.avi");
		camera_set = make_shared<CameraSetVirtual>(name);
	}

	int n_cameras = camera_set->getNumCamera();
	cout << "cameras: " << n_cameras << endl;

#if 1
	// for (int i = 0; i < 100; i ++){
	while (1) {
		// vector<Mat> frame(n_cameras);
		// for(int i=0; i<n_cameras; ++i){
		//           			camera_set->read(frame[i], i);
		// }
		// Mat merged_frame = mergeFrame(frame);
		//           		imshow("frame", merged_frame);
		Mat frame;

		for (int i = 0; i < n_cameras; ++i) {
			camera_set->read(frame, i);
			resize(frame, frame, Size(frame.cols / scale, frame.rows / scale));
			imshow("frame" + to_string(i), frame);
		}
		waitKey(33);
	}
#endif
	return 0;
}
int record(int argc, char** argv) {
	assert(argc >= 2);
	string path(argv[1]);

	shared_ptr<CameraSetBase> camera_set;
	camera_set = make_shared<CameraSetFly2>();

	// int n_cameras = camera_set->getNumCamera();
	int n_cameras = 1;
	cout << "cameras: " << n_cameras << endl;
	if (n_cameras < 1) {
		cerr << "no camera!" << endl;
		return -1;
	}

	Mat frame;
	assert(camera_set->read(frame, 0));
	Size size = frame.size();

	vector<VideoWriter> writer(n_cameras);


	int fps = 5;
	int ms = 1000 / fps;


	for (int i = 0; i < n_cameras; ++i) {
		writer[i].open( path + to_string(i) + ".avi", CV_FOURCC('M', 'J', 'P', 'G'), fps,  size);
		assert(writer[i].isOpened());
	}

	const int scale = 8;

	for (int t = 0; t < 100; ++t) {
		clock_t start_time = clock();
		cout << "time: " << t << endl;
		for (int i = 0 ; i < n_cameras; ++i) {
			assert(camera_set->read(frame, i));
			writer[i] << frame;

			Mat tmp;
			resize(frame, tmp, Size(frame.cols / scale, frame.rows / scale));
			imshow("frame" + to_string(i), tmp	);
		}
		int delay = ms - (clock() - start_time) / 1000 ;
		cout << "delay : " << delay << endl;
		delay = max(1, delay);
		cout << "delay : " << delay << endl;
		waitKey(delay);
	}

	for (int i = 0; i < n_cameras; ++i) {
		writer[i].release();
	}
	return 0;
}
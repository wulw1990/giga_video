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
#include "CameraSet.hpp"
#include "CameraVirtual.hpp"

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

	shared_ptr<CameraBase> camera_set;
	if (0) {
		camera_set = make_shared<CameraSet>();
	} else {
		string path = "/media/wuliwei/data/NB_BBNC/giga_video/zijing16/video/";
		vector<string> name;
		name.push_back(path + "MVI_6878/video.avi");
		name.push_back(path + "MVI_6880/video.avi");
		name.push_back(path + "MVI_6881/video.avi");
		name.push_back(path + "MVI_6883/video.avi");
		camera_set = make_shared<CameraVirtual>(name);
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

		int scale = 4;

		camera_set->read(frame, 0);
		resize(frame, frame, Size(frame.cols / scale, frame.rows / scale));
		imshow("frame0", frame);

		camera_set->read(frame, 1);
		resize(frame, frame, Size(frame.cols / scale, frame.rows / scale));
		imshow("frame1", frame);

		camera_set->read(frame, 2);
		resize(frame, frame, Size(frame.cols / scale, frame.rows / scale));
		imshow("frame2", frame);

		camera_set->read(frame, 3);
		resize(frame, frame, Size(frame.cols / scale, frame.rows / scale));
		imshow("frame3", frame);

		waitKey(33);
	}
#endif
	return 0;
}
int record(int argc, char** argv) {
	assert(argc >= 2);
	string path(argv[1]);

	shared_ptr<CameraBase> camera_set;
	camera_set = make_shared<CameraSet>();

	int n_cameras = camera_set->getNumCamera();
	cout << "cameras: " << n_cameras << endl;
	if (n_cameras < 1) {
		cerr << "no camera!" << endl;
		return -1;
	}

	Mat frame;
	assert(camera_set->read(frame, 0));
	Size size = frame.size();

	vector<VideoWriter> writer(n_cameras);



	for (int i = 0; i < n_cameras; ++i) {
		writer[i].open( path + to_string(i) + ".avi", CV_FOURCC('M', 'J', 'P', 'G'), 15,  size);
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
		int delay = 66 - (clock() - start_time) ;
		delay = max(1, delay);
		cout << "delay : " << delay << endl;
		waitKey(delay);
	}

	for (int i = 0; i < n_cameras; ++i) {
		writer[i].release();
	}
	return 0;
}
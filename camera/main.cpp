#include <string>
#include <vector>
#include <iostream>
#include <cassert>
#include <algorithm>
using namespace std;

//#include "SocketServer.hpp"
#include "CameraSet.hpp"

int demo(int argc, char** argv);

int main(int argc, char **argv) {
	assert(argc >= 2);
	string mode = string( argv[1] );
	argc--;
	argv++;

	transform(mode.begin(), mode.end(), mode.begin(), ::tolower);

	if (mode == "demo") demo(argc, argv);
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
	CameraSet camera_set;

	int n_cameras = camera_set.getNumCamera();
	cout << "cameras: " << n_cameras << endl;

#if 1
	// for (int i = 0; i < 100; i ++){
	while (1) {
		// vector<Mat> frame(n_cameras);
		// for(int i=0; i<n_cameras; ++i){
		//           			camera_set.getCapture(frame[i], i);
		// }
		// Mat merged_frame = mergeFrame(frame);
		//           		imshow("frame", merged_frame);
		Mat frame;

		int scale = 4;

		camera_set.getCapture(frame, 0);
		resize(frame, frame, Size(frame.cols / scale, frame.rows / scale));
		imshow("frame0", frame);

		camera_set.getCapture(frame, 1);
		resize(frame, frame, Size(frame.cols / scale, frame.rows / scale));
		imshow("frame1", frame);

		camera_set.getCapture(frame, 2);
		resize(frame, frame, Size(frame.cols / scale, frame.rows / scale));
		imshow("frame2", frame);

		camera_set.getCapture(frame, 3);
		resize(frame, frame, Size(frame.cols / scale, frame.rows / scale));
		imshow("frame3", frame);

		waitKey(33);
	}
#endif
	return 0;
}
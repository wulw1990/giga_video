#include <string>
#include <vector>
#include <cassert>
#include <iostream>
#include <algorithm>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;

#include "Data.hpp"

int test_multi_video_data(int argc, char** argv);

int main(int argc, char **argv){
	assert(argc >= 2);
	string mode = string( argv[1] );
	argc--;
	argv++;

	transform(mode.begin(), mode.end(), mode.begin(), ::tolower);

	if(mode == "test_multi_video_data") test_multi_video_data(argc, argv);
	else {
		cerr << "main mode error : " << mode << endl;
		return -1;
	}
	return 0;
}
int test_multi_video_data(int argc, char** argv){
	assert(argc>=2);

	MultiVideoData data( argv[1] );
	cout << data.getRectOnScene(0) << endl;
	while(1){
		Mat frame = data.getFrame(0);
		resize(frame, frame, Size(frame.cols/4, frame.rows/4));
		imshow("frame", frame);
		waitKey(33);
	}
	return 0;
}
int video(int argc, char** argv){
	cout << "video demo" << endl;
	return 0;
}
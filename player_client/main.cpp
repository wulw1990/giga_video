#include <string>
#include <vector>
#include <iostream>
#include <cassert>
#include <algorithm>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;

#include "SocketClient.hpp"

int demo(int argc, char** argv);

int main(int argc, char **argv){
	assert(argc >= 2);
	string mode = string( argv[1] );
	argc--;
	argv++;

	transform(mode.begin(), mode.end(), mode.begin(), ::tolower);

	if(mode == "demo") demo(argc, argv);
	else {
		cerr << "main mode error : " << mode << endl;
		return -1;
	}
	return 0;
}
Mat readImage(string name){
	FILE* fp = fopen(name.c_str(), "rb");
	assert(fp);
	fseek(fp, 0L, SEEK_END);
	long sz = ftell(fp);
	printf("size: %ld\n", sz);
	fseek(fp, 0L, SEEK_SET);

	vector<unsigned char> buffer(sz);
	size_t n = fread(&buffer[0], 1, sz, fp);
	cout << "n: " << n << endl;

	Mat rawData  =  Mat( 1, sz, CV_8UC1, &buffer[0] );

	Mat decodedImage  =  imdecode( rawData, 1 /*, flags */ );
	//TODO imencode
	if ( decodedImage.data == NULL ) {
	return Mat();
	}
	imshow("img", decodedImage);
	waitKey(0);
	return decodedImage;
}
int demo(int argc, char** argv){
	cout << "client demo" << endl;
	// init_client(argc, argv);
	readImage("/home/wlw/Pictures/dasheng.jpg");
	return 0;
}
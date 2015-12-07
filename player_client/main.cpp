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
int demo(int argc, char** argv){
	cout << "client demo" << endl;
	assert(argc>=2);
	SocketClient client(argv[1], 5005);
	client.work();
	// readImage("/home/wuliwei/Pictures/type.jpg");
	return 0;
}
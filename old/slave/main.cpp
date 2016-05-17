#include <string>
#include <vector>
#include <iostream>
#include <cassert>
#include <algorithm>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;

#include "Slave.hpp"

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
	assert(argc>=4);
	cout << "slave" << endl;
	Slave client(argv[1], argv[2], atoi(argv[3]), argv[4]);
	client.work();
	return 0;
}

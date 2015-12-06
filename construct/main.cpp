#include <string>
#include <vector>
#include <cassert>
#include <iostream>
#include <algorithm>

using namespace std;

#include "Constructor.hpp"
#include "GeometryAligner.h"
#include "DirDealer.h"

int construct_from_autopan(int argc, char** argv);
int construct_video(int argc, char** argv);
int cut_video(int argc, char** argv);
int test_geo_align(int argc, char** argv);

int main(int argc, char **argv) {
	assert(argc >= 2);
	string mode = string( argv[1] );
	argc--;
	argv++;

	transform(mode.begin(), mode.end(), mode.begin(), ::tolower);

	if (mode == "construct_from_autopan") construct_from_autopan(argc, argv);
	else if (mode == "construct_video") construct_video(argc, argv);
	else if (mode == "cut_video") cut_video(argc, argv);
	else if (mode == "test_geo_align") test_geo_align(argc, argv);
	else {
		cerr << "main mode error : " << mode << endl;
		return -1;
	}
	return 0;
}
int construct_from_autopan(int argc, char** argv) {
	cout << "construct_from_autopan demo" << endl;
	assert(argc >= 2);
	Constructor constructor;
	constructor.writeInfoForAutopan(argv[1]);
	return 0;
}
int construct_video(int argc, char** argv) {
	cout << "construct_video demo" << endl;
	assert(argc >= 4);
	DirDealer dir_dealer;
	dir_dealer.mkdir_p(argv[3]);

	Constructor constructor;
	constructor.constructVideo(argv[1], argv[2], argv[3]);
	return 0;
}
int cut_video(int argc, char** argv) {
	cout << "cut_video demo" << endl;
	assert(argc >= 3);
	string name_src(argv[1]);
	string name_dst(argv[2]);

	Constructor constructor;
	constructor.cutVideo(name_src, name_dst);
	return 0;
}
int test_geo_align(int argc, char** argv) {
	assert(argc >= 3);
	Mat scene = imread(argv[1]);
	Mat frame = imread(argv[2]);
	cv::Mat H;
	cv::Size size;
	cv::Point offset;

	GeometryAligner aligner;
	bool is_matched = aligner.align(frame, scene, H, size, offset);
	cout << "is_matched: " << is_matched << endl;

	if (is_matched) {
		Mat scene1 = scene.clone();
		warpPerspective(frame, scene1, H, scene1.size());
		for (int r = 0; r < scene.rows; ++r) {
			for (int c = 0; c < scene.cols; ++c) {
				if (scene1.at<Vec3b>(r, c) != Vec3b(0, 0, 0)) {
					scene.at<Vec3b>(r, c) = scene1.at<Vec3b>(r, c);
				}
			}
		}
		imwrite("../new_scene.jpg", scene);
	}


	return 0;
}
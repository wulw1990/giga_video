#include <string>
#include <vector>
#include <cassert>
#include <iostream>
#include <algorithm>

using namespace std;

#include "Constructor.hpp"
#include "GeometryAligner.h"
#include "DirDealer.h"
#include "CameraVirtual.hpp"
#include "CameraSet.hpp"
#include "GigaAligner.h"
#include "IO.hpp"

int construct_from_autopan(int argc, char** argv);
int construct_video(int argc, char** argv);
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
	else if (mode == "construct_video") construct_video(argc, argv);
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
	assert(argc >= 2);
	string path(argv[1]);

#if 0
	vector<string> video_name;
	video_name.push_back(path + "video/data/0.avi");
	video_name.push_back(path + "video/data/1.avi");
	CameraVirtual camera_set(video_name);
#else
	CameraSet camera_set;
#endif

	int n_cameras = camera_set.getNumCamera();
	vector<Mat> frame(n_cameras);
	for (int i = 0; i < n_cameras; ++i) {
		assert(camera_set.read(frame[i], i));
		// imshow("frame", frame[i]);
		// waitKey(0);
	}

	for (int i = 0; i < n_cameras; ++i) {
		// for (size_t i = 0; i < 1; ++i) {
		Mat trans;
		Rect rect;
		GigaAligner aligner;
		if (aligner.alignFrameToScene(path, frame[i], trans, rect)) {
			std::ofstream fout;
			assert(IO::openOStream(fout, path + "video/" + to_string(i) + ".txt", "VideoData save"));
			assert(IO::saveTransMat(fout, trans));
			assert(IO::saveRect(fout, rect));
		}
	}

	return 0;
}
#include "CameraSetRecorder.hpp"

#include <opencv2/opencv.hpp>
#include <vector>
using namespace cv;
using namespace std;

#include "DirDealer.h"
#include "Timer.hpp"
#include "CameraSetBase.hpp"

CameraSetRecorder::CameraSetRecorder(std::shared_ptr<CameraSetBase> camera_set) {
	m_camera_set = camera_set;
	m_thread.resize(m_camera_set->getNumCamera());
}
bool CameraSetRecorder::record(std::string path, int n_frames) {
	int n_cameras = m_camera_set->getNumCamera();
	if (n_cameras < 1) {
		cerr << "no camera!" << endl;
		exit(-1);
	}

	int fps = 30;
	int MS = 1000 / fps;

	cout << "Start to read images to RAM ..." << endl;
	vector<vector<Mat> > imgs(n_cameras);
	for (int i = 0 ; i < n_cameras; ++i) {
		imgs[i].resize(n_frames);
	}
	for (int t = 0; t < n_frames; ++t) {
		Timer timer;
		int ms;
		timer.reset();
		for (int i = 0 ; i < n_cameras; ++i) {
			assert(m_camera_set->read(imgs[i][t], i));
		}
		ms = timer.getTimeUs() / 1000;
		cout << "t: " << t << " ms: " << ms << endl;
		if (ms < MS ) {
			std::this_thread::sleep_for(std::chrono::milliseconds(MS - ms));
		}
	}
	cout << "Start to write images to disk ..." << endl;
	vector<string> path_vec(n_cameras);
	for (int i = 0; i < n_cameras; ++i) {
		path_vec[i] = path + to_string(i) + "/";
		DirDealer dir_dealer;
		dir_dealer.mkdir_p(path_vec[i]);
	}
	for (int t = 0; t < n_frames; ++t) {
		for (int i = 0 ; i < n_cameras; ++i) {
			string name = path_vec[i] + to_string(t) + ".jpg";
			cout << "Write image: " << name << endl;
			imwrite(name, imgs[i][t]);
		}
	}
	cout << "write ok" << endl << endl;
	return true;
}

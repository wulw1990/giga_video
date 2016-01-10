#include "CameraSetRecorder.hpp"

#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

#include "DirDealer.h"
#include "Timer.hpp"
#include "CameraSetBase.hpp"

static void internal_save(string& name, cv::Mat& frame) {
	// cout << "name: " << name << endl;
	imwrite(name, frame);
}
CameraSetRecorder::CameraSetRecorder(std::shared_ptr<CameraSetBase> camera_set) {
	m_camera_set = camera_set;
	m_thread.resize(m_camera_set->getNumCamera());
}
bool CameraSetRecorder::record(std::string path, int n_frames) {
	int n_cameras = m_camera_set->getNumCamera();
	//int n_cameras = 4;
	if (n_cameras < 1) {
		cerr << "no camera!" << endl;
		return -1;
	}
	vector<string> path_vec(n_cameras);
	for (int i = 0; i < n_cameras; ++i) {
		path_vec[i] = path + to_string(i) + "/";
		DirDealer dir_dealer;
		dir_dealer.mkdir_p(path_vec[i]);
	}

	// vector<VideoWriter> writer(n_cameras);
	int fps = 4;
	int MS = 1000 / fps;
	// cout << "*************n_cameras = " << n_cameras << "********************" <<endl;
	// for (int i = 0; i < n_cameras; ++i) {
	// 	writer[i].open( path + format ("%d", i)+ ".avi", CV_FOURCC('M', 'J', 'P', 'G'), fps,  size);
	// 	cout << "*************************************" <<endl;
	// 	cout << path + to_string(i) + ".avi" << endl;
	// 	cout << "*************************************" <<endl;
	// 	assert(writer[i].isOpened());
	// }

	Timer timer;
	vector<Mat> frame(n_cameras);
	vector<string> name(n_cameras);

	for (int i = 0; i < n_cameras; ++i) {
		camera_set->setShutter(3.0, i);
	}

	for (int t = 0; t < n_frames; ++t) {
		timer.reset();
		for (int i = 0 ; i < n_cameras; ++i) {
			assert(m_camera_set->read(frame[i], i));
		}
		for (int i = 0 ; i < n_cameras; ++i) {
			// 	if (i == 0) {
			// 		camera_set->setShutter(0.5 + (t / 5) % 10, i);
			// 	}
			// m_camera_set->read(frame, i);
			// timer.reset();
			// cout << "read time: " << timer.getTimeUs() / 1000 << "\t";
			// cout.flush();
			// timer.reset();
			// imwrite(path_vec[i] + to_string(t) + ".jpg", frame);
			name[i] = path_vec[i] + to_string(t) + ".jpg";
			m_thread[i] = std::thread(internal_save, std::ref(name[i]), std::ref(frame[i]));
			// cout << "write time: " << timer.getTimeUs() / 1000 << "\t";
			// cout.flush();
		}
		for (int i = 0 ; i < n_cameras; ++i) {
			m_thread[i].join();
		}
		int ms = timer.getTimeUs() / 1000;
		cout << "time: " << t << "\tms: " <<  ms << "\tdelay: " << MS - ms << endl;
		if (ms < MS ) {
			waitKey(MS - ms);
		}
	}
	cout << "record ok" << endl << endl;
	// for (int i = 0 ; i < n_cameras; ++i) {
	// 	m_thread[i].detach();
	// }
	return true;
}

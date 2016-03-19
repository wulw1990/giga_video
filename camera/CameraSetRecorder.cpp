#include "CameraSetRecorder.hpp"

#include <opencv2/opencv.hpp>
#include <vector>
using namespace cv;
using namespace std;

#include "DirDealer.h"
#include "Timer.hpp"
#include "CameraSetBase.hpp"


static void internal_save(string& name, cv::Mat& frame, cv::Mat& imgs) {
	//imwrite(name, frame);
    imgs = frame.clone();
}
CameraSetRecorder::CameraSetRecorder(std::shared_ptr<CameraSetBase> camera_set) {
	m_camera_set = camera_set;
	m_thread.resize(m_camera_set->getNumCamera());
}
bool CameraSetRecorder::record(std::string path, int n_frames) {
	int n_cameras = m_camera_set->getNumCamera();
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

	int fps = 10;
	int MS = 1000 / fps;

	Timer timer;
	vector<Mat> frame(n_cameras);
	vector<string> name(n_cameras);

    cout << "Start to read images to RAM ..." << endl;
    vector<vector<Mat>> imgs;
    imgs.resize(n_cameras);
    for (int i = 0 ; i < n_cameras; ++i) {
		imgs[i].resize(n_frames);
	}
    
    int ms;
	for (int t = 0; t < n_frames; ++t) {
		timer.reset();
		for (int i = 0 ; i < n_cameras; ++i) {
			assert(m_camera_set->read(frame[i], i));
		}
        ms = timer.getTimeUs() / 1000;
        cout << endl << "read frame:" << ms << "ms" << endl;
		for (int i = 0 ; i < n_cameras; ++i) {
			name[i] = path_vec[i] + to_string(t) + ".jpg";
			m_thread[i] = std::thread(internal_save, std::ref(name[i]), std::ref(frame[i]), std::ref(imgs[i][t]));
		}
        ms = timer.getTimeUs() / 1000;
        cout << "add frame to vectors:" << ms << "ms" << endl;
		for (int i = 0 ; i < n_cameras; ++i) {
			m_thread[i].join();
		}
        ms = timer.getTimeUs() / 1000;
        cout << "join thread:" << ms << "ms" << endl;
		ms = timer.getTimeUs() / 1000;
		cout << "time: " << t << "\tms: " <<  ms << "\tdelay: " << MS - ms << endl;
		if (ms < MS ) {
			waitKey(MS - ms);
		}
	}
    
    cout << "Start to write images to disk ..." << endl;
    for (int t = 0; t < n_frames; ++t) {
        for (int i = 0 ; i < n_cameras; ++i) {
			name[i] = path_vec[i] + to_string(t) + ".jpg";
            cout << "Write image: " << name[i] << endl;
			imwrite(name[i], imgs[i][t]);
		}
    }
    
	cout << "record ok" << endl << endl;
	return true;
}

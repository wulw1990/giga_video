#include "Player.hpp"
using namespace std;
using namespace cv;

#include "FrameProvider.hpp"
#include "WindowController.hpp"
#include "Timer.hpp"

const string win_title = "giga player";
const int w = 1700;
const int h = 900;

const int FPS = 5;
const int MS = 1000/FPS;

Player::Player(std::string path, int n_frames, string output_video ) {
	m_frame_provider.clear();
	for (int i = 0; i < n_frames; ++i) {
		m_frame_provider.push_back(make_shared<FrameProvider>(path + to_string(i) + "/", false));
	}

	int n_layers = m_frame_provider[0]->getNumLayers();
	Size top_layer_size(m_frame_provider[0]->getLayerWidth(n_layers - 1), m_frame_provider[0]->getLayerHeight(n_layers - 1));
	Size winsize(w, h);
	m_info.m_window_controller = make_shared<WindowController>(n_layers, top_layer_size, winsize);

	m_info.update = true;
	m_info.sZoom = 0;
	m_frame_id = 0;

	if(output_video!=""){
		m_video_writer.open( output_video, CV_FOURCC('M', 'J', 'P', 'G'), FPS,  winsize);
		assert(m_video_writer.isOpened());
	}
}
void Player::play() {
	Timer timer;

	while (1) {
		timer.reset();
		double x, y, z;
		if(m_info.sZoom != 0.0)
		{
			m_info.m_window_controller->zoom(0.1*(double)m_info.sZoom/abs(m_info.sZoom));
			m_info.sZoom -= m_info.sZoom/abs(m_info.sZoom);
		}
		m_info.m_window_controller->getXYZ(x, y, z);
		// cout << "x: " << x << "\ty: " << y << "\tz: " << z << endl;

		Mat frame, mask;
		m_frame_provider[m_frame_id]->getFrameWithMask(frame, mask, w, h, x, y, z);
		imshow(win_title, frame);
		// imshow(win_title + "-mask", mask);

		cv::setMouseCallback(win_title, onMouse, &m_info);
		char key = waitKey(1);
		if(key=='q') break;

		m_video_writer << frame;

		int ms = timer.getTimeUs() / 1000;
		// cout << "ms: " << ms << endl;
		if (ms < MS && m_info.sZoom == 0) {
			waitKey(MS - ms);
		}

		m_frame_id++;
		if(m_frame_id==(int)m_frame_provider.size()) m_frame_id=0;
		// cout << "time: " << timer.getTimeUs()/1000 << " ms" << endl;
	}
	m_video_writer.release();
}
void Player::onMouse(int event, int x, int y, int, void* data)
{
	static bool sLButtonDown = false;
	Mat frame,mask;
	if (event != EVENT_LBUTTONDOWN
	        && event != EVENT_LBUTTONUP
	        && event != EVENT_LBUTTONDBLCLK
	        && event != EVENT_RBUTTONDBLCLK
	        && event != EVENT_MOUSEMOVE)
		return;
	Info* info = (Info*)data;
	
	if(info->sZoom != 0)
	{
		return;
	}
	switch (event)
	{
	case EVENT_LBUTTONDOWN:
		sLButtonDown = true;
		info->pre_x = x;
		info->pre_y = y;
		break;
	case EVENT_LBUTTONUP:
		sLButtonDown = false;
		info->m_window_controller->move(info->pre_x - x, info->pre_y - y);
		break;
	case EVENT_LBUTTONDBLCLK:
		info->sZoom = 10;
		sLButtonDown = false;
		break;
	case EVENT_RBUTTONDBLCLK:
		info->sZoom = -10;
		sLButtonDown = false;
		break;
	case EVENT_MOUSEMOVE:
		if(sLButtonDown)
		{
			info->m_window_controller->move(info->pre_x - x, info->pre_y - y);
			info->pre_x = x;
			info->pre_y = y;
		}
		break;
	}
	info->update = true;
}

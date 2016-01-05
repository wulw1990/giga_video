#include "Player.hpp"
using namespace std;
using namespace cv;

#include "FrameProvider.hpp"
#include "WindowController.hpp"
#include "Timer.hpp"

const string win_title = "giga player";
const int w = 1700;
const int h = 900;

const int MS = 200;

Player::Player(std::string path, bool enable_video ) {
	m_info.m_frame_provider = make_shared<FrameProvider>(path, enable_video);

	int n_layers = m_info.m_frame_provider->getNumLayers();
	Size top_layer_size(m_info.m_frame_provider->getLayerWidth(n_layers - 1), m_info.m_frame_provider->getLayerHeight(n_layers - 1));
	Size winsize(1000, 500);
	m_info.m_window_controller = make_shared<WindowController>(n_layers, top_layer_size, winsize);

	m_info.update = true;
}
void Player::play() {
	Timer timer;
	while (1) {
		timer.reset();
		double x, y, z;
		m_info.m_window_controller->getXYZ(x, y, z);
		// cout << "x: " << x << "\ty: " << y << "\tz: " << z << endl;

		Mat frame, mask;
		m_info.m_frame_provider->getFrameWithMask(frame, mask, w, h, x, y, z);
		imshow(win_title, frame);
		// imshow(win_title + "-mask", mask);

		cv::setMouseCallback(win_title, onMouse, &m_info);
		waitKey(1);

		int ms = timer.getTimeUs()/1000;
		// cout << "ms: " << ms << endl;
		if(ms < MS){
			waitKey(MS - ms);
		}
		// cout << "time: " << timer.getTimeUs()/1000 << " ms" << endl;
	}
}
void Player::onMouse(int event, int x, int y, int, void* data)
{
	if (event != EVENT_LBUTTONDOWN
	        && event != EVENT_LBUTTONUP
	        && event != EVENT_LBUTTONDBLCLK
	        && event != EVENT_RBUTTONDBLCLK)
		return;
	Info* info = (Info*)data;
	switch (event)
	{
	case EVENT_LBUTTONDOWN:
		info->pre_x = x;
		info->pre_y = y;
		break;
	case EVENT_LBUTTONUP:
		info->m_window_controller->move(info->pre_x - x, info->pre_y - y);
		break;
	case EVENT_LBUTTONDBLCLK:
		info->m_window_controller->zoom(1);
		break;
	case EVENT_RBUTTONDBLCLK:
		info->m_window_controller->zoom(-1);
		break;
	}
	info->update = true;
}

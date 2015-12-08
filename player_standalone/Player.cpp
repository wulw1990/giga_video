#include "Player.hpp"
using namespace std;
using namespace cv;

#include "DataProvide.hpp"
#include "WindowController.hpp"

Player::Player(std::string path, bool enable_video ) {
	m_info.win_title = "giga  player";
	m_info.w = 1000;
	m_info.h = 500;
	m_info.m_frame_provider = make_shared<FrameProvider>(path, enable_video);

	int n_layers = m_info.m_frame_provider->getNumLayers();
	Size top_layer_size(m_info.m_frame_provider->getLayerWidth(n_layers - 1), m_info.m_frame_provider->getLayerHeight(n_layers - 1));
	Size winsize(1000, 500);
	m_info.m_window_controller = make_shared<WindowController>(n_layers, top_layer_size, winsize);

	m_info.update = true;
}
void Player::play() {
	while (1) {
		if (m_info.update) {
			double x, y, z;
			m_info.m_window_controller->getXYZ(x, y, z);
			// cout << x << "\t" << y << "\t" << z << endl;
			m_info.frame = m_info.m_frame_provider->getFrame(m_info.w, m_info.h, x, y, z);
			imshow(m_info.win_title, m_info.frame);
			cv::setMouseCallback(m_info.win_title, onMouse, &m_info);

			m_info.update = false;
		}
		waitKey(30);
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
		info->m_window_controller->zoom(0.5);
		break;
	case EVENT_RBUTTONDBLCLK:
		info->m_window_controller->zoom(-0.5);
		break;
	}
	info->update = true;
	// double qx, qy, qz;
	// info->m_window_controller->getXYZ(qx, qy, qz);
	// cout << qx << "\t" << qy << "\t" << qz << endl;
	// info->frame = info->m_frame_provider->getFrame(info->w, info->h, qx, qy, qz);
}
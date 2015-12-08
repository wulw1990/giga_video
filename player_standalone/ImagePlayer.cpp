#include "ImagePlayer.hpp"
using namespace std;
using namespace cv;

#include "DataProvide.hpp"
#include "WindowController.hpp"

ImagePlayer::ImagePlayer(std::string path, std::string info_file ) {
	m_info.m_window_title = "giga image player";
	m_info.w = 1000;
	m_info.h = 500;
	m_info.x = 0.5;
	m_info.y = 0.5;
	m_info.z = 0;
	m_info.m_frame_provider = make_shared<SceneFrameProvider>(path, info_file);

	int n_layers = 4;
	Size top_layer_size(m_info.m_frame_provider->getLayerWidth(4), m_info.m_frame_provider->getLayerHeight(4));
	Size winsize(1000, 500);
	m_info.m_window_controller = make_shared<WindowController>(n_layers, top_layer_size, winsize);
}
void ImagePlayer::play() {
	cout << "ImagePlayer::play" << endl;
	m_info.frame = m_info.m_frame_provider->getFrame(m_info.w, m_info.h, m_info.x, m_info.y, m_info.y);

	// double x, y, z;
	// m_info.m_window_controller->getXYZ(x, y, z);
	// cout << x << " " << y << " " << z << endl;
	// return;

	while (1) {
		double x, y, z;
		m_info.m_window_controller->getXYZ(x, y, z);
		m_info.frame = m_info.m_frame_provider->getFrame(m_info.w, m_info.h, x, y, z);

		imshow(m_info.m_window_title, m_info.frame);
		cv::setMouseCallback(m_info.m_window_title, onMouse, &m_info);
		waitKey(30);
	}
}
void ImagePlayer::onMouse(int event, int x, int y, int, void* data)
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
	double qx, qy, qz;
	info->m_window_controller->getXYZ(qx, qy, qz);
	// cout << qx << "\t" << qy << "\t" << qz << endl;
	info->frame = info->m_frame_provider->getFrame(info->w, info->h, qx, qy, qz);
}
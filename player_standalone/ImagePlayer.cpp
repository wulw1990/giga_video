#include "ImagePlayer.hpp"
#include "DataProvide.hpp"
using namespace std;
using namespace cv;

ImagePlayer::ImagePlayer(std::string path, std::string info_file ) {
	m_info.m_window_title = "giga image player";
	m_window_width = 1000;
	m_window_height = 500;
	m_info.x = 0.5;
	m_info.y = 0.5;
	m_info.z = 0;
	m_info.m_frame_provider = make_shared<SceneFrameProvider>(path, info_file, m_window_width, m_window_height);
}
void ImagePlayer::play() {
	cout << "ImagePlayer::play" << endl;
	m_info.frame = m_info.m_frame_provider->getFrame(m_info.x, m_info.y, m_info.y);
	while (1) {
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
		// cout << info->x << "\t" << info->y << endl;
		info->m_frame_provider->incXY(info->z, info->pre_x - x, info->pre_y - y, info->x, info->y);
		info->frame = info->m_frame_provider->getFrame(info->x, info->y, info->z);
		// cout << info->x << "\t" << info->y << endl;

		break;
	case EVENT_LBUTTONDBLCLK:
	cout << "EVENT_LBUTTONDBLCLK" << endl;
		// cout << info->z << endl;
		info->z += 0.5;
		info->z = min(info->z, info->m_frame_provider->getMaxZ());
		// cout << info->z << endl;
		info->frame = info->m_frame_provider->getFrame(info->x, info->y, info->z);
		break;
	case EVENT_RBUTTONDBLCLK:
	cout << "EVENT_RBUTTONDBLCLK" << endl;
		info->z -= 0.5;
		info->z = max(info->z, info->m_frame_provider->getMinZ());
		info->frame = info->m_frame_provider->getFrame(info->x, info->y, info->z);
		break;
	}
}
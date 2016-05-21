#ifndef VIDEO_PROVIDER_HPP_
#define VIDEO_PROVIDER_HPP_

#include <vector>
#include <string>
#include <fstream>
#include <memory>
#include <opencv2/opencv.hpp>

class CameraSetBase;

class VideoProvider{
	public:
		VideoProvider(std::string path, bool online = false);
		int getNumCamera();
		bool getRectOnScene(cv::Rect& rect, int layer_id, int camera_id);
		bool getFrame(cv::Mat& frame, int layer_id, int camera_id);

	private:
		std::string m_path;
		std::vector<std::shared_ptr<CameraSetBase> > m_camera_set;

		std::vector<std::vector<cv::Mat>> m_trans;
		std::vector<std::vector<cv::Rect>> m_rect;
		
		bool isValidLayer(int layer_id);
		bool isValidCamera(int camera_id);
};

#endif
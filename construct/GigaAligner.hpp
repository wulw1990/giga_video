#ifndef GIGA_ALIGNER_H_
#define GIGA_ALIGNER_H_

#include <opencv2/opencv.hpp>
#include <string>
#include <memory>

class GeometryAligner;
class FrameProvider;


class GigaAligner
{
public:
	GigaAligner(std::string path);
	bool alignFrame(cv::Mat frame, cv::Mat& H, cv::Rect& rect_on_scene);
	//TODO
	//void manualSelect(std::string path_scene, std::string input_video);

private:
	std::shared_ptr<GeometryAligner> m_geometry_aligner;
	std::shared_ptr<FrameProvider> m_frame_provider;

	void showImage(std::string win_name, cv::Mat img);
};

#endif
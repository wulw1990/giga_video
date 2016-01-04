#ifndef GIGA_ALIGNER_BASE_HPP_
#define GIGA_ALIGNER_BASE_HPP_

#include <opencv2/opencv.hpp>
#include <string>
#include <memory>
class FrameProvider;

class GigaAlignerBase
{
public:
	GigaAlignerBase(std::string path);
	bool align(cv::Mat frame, cv::Mat& trans, cv::Rect& rect);
	virtual bool alignToWin(cv::Mat frame, cv::Mat win, cv::Mat& trans, cv::Rect& rect) = 0;

protected:
	std::shared_ptr<FrameProvider> m_frame_provider;
};

#endif
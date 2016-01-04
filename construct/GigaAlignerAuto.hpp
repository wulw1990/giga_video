#ifndef GIGA_ALIGNER_H_
#define GIGA_ALIGNER_H_

#include <opencv2/opencv.hpp>
#include <string>
#include <memory>

#include "GigaAlignerBase.hpp"

class GeometryAligner;
class FrameProvider;

class GigaAlignerAuto: public GigaAlignerBase
{
public:
	GigaAlignerAuto(std::string path);
	bool alignToWin(cv::Mat frame, cv::Mat win, cv::Mat& trans, cv::Rect& rect);

private:
	std::shared_ptr<GeometryAligner> m_geometry_aligner;
};

#endif
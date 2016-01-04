#ifndef GIGA_ALIGNER_MANUALL_HPP_
#define GIGA_ALIGNER_MANUALL_HPP_

#include "GigaAlignerBase.hpp"
#include <string>

class GigaAlignerManual: public GigaAlignerBase
{
public:
	GigaAlignerManual(std::string path):GigaAlignerBase(path){}
	~GigaAlignerManual(){}
	
	bool alignToWin(cv::Mat frame, cv::Mat win, cv::Mat& trans, cv::Rect& rect);
};

#endif
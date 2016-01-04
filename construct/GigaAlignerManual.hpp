#ifndef GIGA_ALIGNER_MANUALL_HPP_
#define GIGA_ALIGNER_MANUALL_HPP_

#include "GigaAlignerBase.hpp"
#include <string>
#include <vector>

class GigaAlignerManual: public GigaAlignerBase
{
public:
	GigaAlignerManual(std::string path):GigaAlignerBase(path){}
	~GigaAlignerManual(){}
	
	bool alignToWin(cv::Mat frame, cv::Mat win, cv::Mat& trans, cv::Rect& rect);
private:
	static void onMouseFrame(int event, int x, int y, int, void* data);
	static void onMouseWin(int event, int x, int y, int, void* data);

	std::vector<cv::Point2f> m_point_frame; 
	std::vector<cv::Point2f> m_point_win;

};

#endif
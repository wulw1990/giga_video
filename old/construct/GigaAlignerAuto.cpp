#include "GigaAlignerAuto.hpp"

#include <iostream>
#include <fstream>
#include <time.h>
using namespace std;
using namespace cv;

#include "GeometryAligner.hpp"
#include "FrameProvider.hpp"
#include "Timer.hpp"

GigaAlignerAuto::GigaAlignerAuto(string path):GigaAlignerBase(path){
	m_geometry_aligner = make_shared<GeometryAligner>();
}
bool GigaAlignerAuto::alignToWin(cv::Mat frame, cv::Mat win, cv::Mat& trans, cv::Rect& rect) {
	return m_geometry_aligner->align(frame, win, trans, rect);
}
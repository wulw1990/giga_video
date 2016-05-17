#ifndef IO_HPP_
#define IO_HPP_

#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>

class IO
{
public:
	static bool openOStream(std::ofstream& fout, std::string name, std::string error);
	static bool openIStream(std::ifstream& fin, std::string name, std::string error);
	
	static bool saveMat(std::ofstream& fout, const cv::Mat& mat);
	static bool loadMat(std::ifstream& fin, cv::Mat& mat);

	static bool saveTransMat(std::ofstream& fout, const cv::Mat& mat);
	static bool loadTransMat(std::ifstream& fin, cv::Mat& mat);

	static bool saveRect(std::ofstream& fout, const cv::Rect& rect);
	static bool loadRect(std::ifstream& fin, cv::Rect& rect);



};

#endif
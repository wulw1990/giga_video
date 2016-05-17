#include "IO.hpp"

using namespace std;
using namespace cv;

bool IO::openOStream(std::ofstream& fout, std::string name, std::string error){
	fout.open(name.c_str());
	if(!fout.is_open()){
		cerr << "error: " + error << endl;
		cerr << "can not open output: " << name << endl;
		return false;
	}
	return true;
}
bool IO::openIStream(std::ifstream& fin, std::string name, std::string error){
	fin.open(name.c_str());
	if(!fin.is_open()){
		cerr << "error: " + error << endl;
		cerr << "can not open input: " << name << endl;
		return false;
	}
	return true;
}
bool IO::saveMat(std::ofstream& fout, const cv::Mat& mat){
	return false;
}
bool IO::loadMat(std::ifstream& fin, cv::Mat& mat){
	return false;
}
bool IO::saveTransMat(std::ofstream& fout, const cv::Mat& mat){
	if(mat.size()!=Size(3,3)){
		return false;
	}
	Mat H = mat.clone();
	H.convertTo(H, CV_32FC1);
	for (int r = 0; r < H.rows; ++r) {
		for (int c = 0; c < H.cols; ++c) {
			fout << H.at<float>(r, c) << "\t";
		}
		fout << endl;
	}
	return true;
}
bool IO::loadTransMat(std::ifstream& fin, cv::Mat& mat){
	mat = Mat(3, 3, CV_32FC1);
	for (int r = 0; r < mat.rows; ++r) {
		for (int c = 0; c < mat.cols; ++c) {
			if(!(fin >> mat.at<float>(r, c))){
				return false;
			}
		}
	}
	return true;
}
bool IO::saveRect(std::ofstream& fout, const cv::Rect& rect){
	fout << rect.x << "\t";
	fout << rect.y << "\t";
	fout << rect.width << "\t";
	fout << rect.height << "\t";
	return true;
}
bool IO::loadRect(std::ifstream& fin, cv::Rect& rect){
	return(fin >> rect.x >> rect.y >> rect.width >> rect.height);
}

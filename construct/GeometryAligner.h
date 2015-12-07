#ifndef _GEOMETRY_ALIGNER_H_
#define _GEOMETRY_ALIGNER_H_

#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;

class GeometryAligner
{
public:
	bool align(cv::Mat frame, cv::Mat scene, cv::Mat& H, cv::Rect& rect_on_scene);
private:
	bool align(Mat frame, Mat scene, Mat& dst_frame, Rect& dst_rect, Mat& T);
	
	void writeMatchResult(
		Mat frame, Mat scene,
		vector<KeyPoint>& keypoints_frame,
		vector<KeyPoint>& keypoints_scene,
		vector<DMatch>& good_match,
		Mat H,
		string name
		);
	bool imageMatchSurf(Mat frame, Mat scene, Mat& H, int thresh = 3);
	cv::Mat cp2rigid(vector<cv::Point2f> src, vector<cv::Point2f> dst);
	void showImage(string win_name, Mat img);

	void selectGoodMatch(vector<DMatch>& match);

	std::vector<cv::Point2f> getCornerOnFrame(cv::Size size);
	std::vector<cv::Point2f> getCornerOnScene(cv::Size size, cv::Mat H);
	
	bool isRectangle(std::vector<cv::Point2f>& corner);
	bool isRightTriAngle(std::vector<cv::Point2f>& corner);
	float getDistance(cv::Point2f p1, cv::Point2f& p2);
	cv::Rect getRectFromCorner(std::vector<cv::Point2f>& corner);

	//TODO
	// bool validCorner(std::vector<cv::Point2f>& corner, cv::Size size);

};
#endif
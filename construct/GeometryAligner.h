#ifndef _GEOMETRY_ALIGNER_H_
#define _GEOMETRY_ALIGNER_H_

#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;

class GeometryAligner
{
public:
	bool align(Mat frame, Mat scene, Mat& dst_frame, Rect& dst_rect, Mat& T);
private:
	bool getKeyPointAndGoodMatch(
		Mat frame, Mat scene,
		vector<KeyPoint>& keypoints_frame,
		vector<KeyPoint>& keypoints_scene,
		vector<DMatch>& good_match);
	void getGoodPoint(
		vector<KeyPoint>& keypoints_frame,
		vector<KeyPoint>& keypoints_scene,
		vector<DMatch>& good_match,
		vector<Point2f>& goodpoint_frame,
		vector<Point2f>& goodpoint_scene);
	void writeMatchResult(
		Mat frame, Mat scene,
		vector<KeyPoint>& keypoints_frame,
		vector<KeyPoint>& keypoints_scene,
		vector<DMatch>& good_match,
		Mat H,
		string name
		);
	void drawRect(Mat img, vector<Point2f>& corner);
	Mat getMatOnScene(Mat frame, Mat scene, Mat H);
	bool imageMatchSurf(Mat frame, Mat scene, Mat& H, int thresh = 3);
	bool isGoodMatch(Mat frame, Mat scene, int thresh);
	cv::Mat cp2rigid(vector<cv::Point2f> src, vector<cv::Point2f> dst);
	void showImage(string win_name, Mat img);
};
#endif
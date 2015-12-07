#include "GeometryAligner.h"
#include <fstream>
#include <time.h>

static const int debug = 1;

bool GeometryAligner::align(cv::Mat frame_, cv::Mat scene_, cv::Mat& H, cv::Rect& rect_on_scene) {
	const int max_width = 500;

	// -- down scale ---
	int scale = 1;
	Mat frame = frame_.clone();
	Mat scene = scene_.clone();
	while (frame.cols > max_width) {
		resize(frame, frame, Size(frame.cols / 2, frame.rows / 2));
		resize(scene, scene, Size(scene.cols / 2, scene.rows / 2));
		scale *= 2;
	}

	if (!imageMatchSurf(frame, scene, H, 15)) {
		return false;
	}

	vector<Point2f> corner_frame = getCornerOnFrame(frame.size());
	vector<Point2f> corner_scene = getCornerOnScene(frame.size(), H);

	for (size_t i = 0; i < corner_frame.size(); ++i) {
		corner_frame[i].x *= scale;
		corner_frame[i].y *= scale;
		corner_scene[i].x *= scale;
		corner_scene[i].y *= scale;
	}

	rect_on_scene = getRectFromCorner(corner_scene);
	// cout << rect_on_scene << endl;
	for(size_t i=0; i<corner_scene.size(); ++i){
		corner_scene[i].x -= rect_on_scene.x;
		corner_scene[i].y -= rect_on_scene.y;
	}

	H = findHomography(corner_frame, corner_scene);

	return true;
}
bool GeometryAligner::align(Mat _frame, Mat _scene, Mat& dst_frame, Rect& dst_rect, Mat& T)
{
	const int max_width = 500;

	// -- down scale ---
	int scale = 1;
	Mat frame = _frame.clone();
	Mat scene = _scene.clone();
	while (frame.cols > max_width) {
		resize(frame, frame, Size(frame.cols / 2, frame.rows / 2));
		resize(scene, scene, Size(scene.cols / 2, scene.rows / 2));
		scale *= 2;
	}

	Mat H;
	if (!imageMatchSurf(frame, scene, H, 15))
		return false;

	const int border = static_cast<int>(0.1 * frame.cols);
	vector<Point2f> obj_corners(4);
	obj_corners[0] = Point(border, border);
	obj_corners[1] = Point(frame.cols - border, border);
	obj_corners[2] = Point(frame.cols - border, frame.rows - border);
	obj_corners[3] = Point(border, frame.rows - border);

	vector<Point2f> scene_corners(4);
	perspectiveTransform(obj_corners, scene_corners, H);
	/*cout << H << endl;
	system("pause");*/

	// -- up scale --
	for (size_t i = 0; i < obj_corners.size(); ++i) {
		obj_corners[i] *= scale;
		scene_corners[i] *= scale;
	}

	int x = (int)scene_corners[0].x;
	int y = (int)scene_corners[0].y;
	int w = (int)norm((Mat)scene_corners[0], (Mat)scene_corners[1], 2);
	int h = (int)norm((Mat)scene_corners[0], (Mat)scene_corners[3], 2);
	dst_rect = Rect(x, y, w, h);

	Rect rect_max(0, 0, _scene.cols, _scene.rows);
	if ((dst_rect & rect_max) != dst_rect)
		return false;

	//update scene_corners
	Point2f start_point = scene_corners[0];
	for (size_t i = 0; i < scene_corners.size(); ++i)
		scene_corners[i] -= start_point;

	T = cp2rigid(obj_corners, scene_corners);
	warpAffine(_frame, dst_frame, T, Size(w, h));
	return true;
}
Mat GeometryAligner::cp2rigid(vector<Point2f> src, vector<Point2f> dst)
{
	int M = (int)src.size();
	assert(src.size() == dst.size());
	Mat X(2 * M, 4, CV_32FC1), U(2 * M, 1, CV_32FC1);
	for (int i = 0; i < M; ++i)
	{
		X.at<float>(i, 0) = src[i].x;
		X.at<float>(i + M, 0) = src[i].y;
		X.at<float>(i, 1) = src[i].y;
		X.at<float>(i + M, 1) = -src[i].x;
		X.at<float>(i, 2) = X.at<float>(i + M, 3) = 1.;
		X.at<float>(i, 3) = X.at<float>(i + M, 2) = 0.;
		U.at<float>(i, 0) = dst[i].x;
		U.at<float>(i + M, 0) = dst[i].y;
	}
	Mat r = X.inv(DECOMP_SVD) * U, trans(2, 3, CV_32FC1);
	trans.at<float>(0, 0) = r.at<float>(0, 0);
	trans.at<float>(0, 1) = r.at<float>(1, 0);
	trans.at<float>(0, 2) = r.at<float>(2, 0);
	trans.at<float>(1, 0) = -r.at<float>(1, 0);
	trans.at<float>(1, 1) = r.at<float>(0, 0);
	trans.at<float>(1, 2) = r.at<float>(3, 0);
	return trans;
}
void GeometryAligner::showImage(string win_name, Mat img)
{
	const int w_max = 1800;
	const int h_max = 900;
	Mat show = img.clone();
	while (show.rows > h_max || show.cols > w_max)
		resize(show, show, Size(show.cols / 2, show.rows / 2));
	imshow(win_name, show);
	waitKey(1);
}

#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/legacy/legacy.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/flann/flann.hpp>

void GeometryAligner::writeMatchResult(
    Mat frame, Mat scene,
    vector<KeyPoint>& keypoint_frame,
    vector<KeyPoint>& keypoint_scene,
    vector<DMatch>& good_match,
    Mat H,
    string name) {

	Mat img_matches;
	drawMatches(frame, keypoint_frame, scene, keypoint_scene,
	            good_match, img_matches, Scalar::all(-1), Scalar::all(-1),
	            vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

	//-- Get the corners from the image_1 ( the object to be "detected" )
	vector<Point2f> obj_corners(4);
	obj_corners[0] = Point2f(0, 0);
	obj_corners[1] = Point2f((float)frame.cols, 0);
	obj_corners[2] = Point2f((float)frame.cols, (float)frame.rows);
	obj_corners[3] = Point2f(0, (float)frame.rows);

	vector<Point2f> scene_corners(4);
	perspectiveTransform(obj_corners, scene_corners, H);

	//-- Draw lines between the corners (the mapped object in the scene - image_2 )
	Point2f offset((float)frame.cols, 0);
	Scalar line_color(0, 0, 255);
	line(img_matches, scene_corners[0] + offset, scene_corners[1] + offset, line_color, 2);
	line(img_matches, scene_corners[1] + offset, scene_corners[2] + offset, line_color, 2);
	line(img_matches, scene_corners[2] + offset, scene_corners[3] + offset, line_color, 2);
	line(img_matches, scene_corners[3] + offset, scene_corners[0] + offset, line_color, 2);

	imwrite(name, img_matches);
}
std::vector<cv::Point2f> GeometryAligner::getCornerOnFrame(cv::Size size) {
	int rows = size.height;
	int cols = size.width;
	vector<Point2f> corner_frame(4);
	corner_frame[0] = Point2f(0, 0);
	corner_frame[1] = Point2f(cols, 0);
	corner_frame[2] = Point2f(cols, rows);
	corner_frame[3] = Point2f(0, rows);

	return corner_frame;
}
std::vector<cv::Point2f> GeometryAligner::getCornerOnScene(cv::Size size, cv::Mat H) {
	vector<Point2f> corner_frame = getCornerOnFrame(size);

	vector<Point2f> corner_on_scene;
	perspectiveTransform(corner_frame, corner_on_scene, H);
	return corner_on_scene;
}
cv::Rect GeometryAligner::getRectFromCorner(std::vector<cv::Point2f>& corner) {
	assert(!corner.empty());
	int x1 = corner[0].x;
	int y1 = corner[0].y;
	int x2 = corner[0].x;
	int y2 = corner[0].y;
	for (size_t i = 0; i < corner.size(); ++i) {
		if (corner[i].x < x1) x1 = corner[i].x;
		if (corner[i].x > x2) x2 = corner[i].x;
		if (corner[i].y < y1) y1 = corner[i].y;
		if (corner[i].y > y2) y2 = corner[i].y;
		cout << corner[i] << endl;
	}
	return Rect(x1, y1, x2 - x1 + 1, y2 - y1 + 1);
}
void GeometryAligner::selectGoodMatch(vector<DMatch>& match) {
	double max_dist = match[0].distance;
	double min_dist = match[0].distance;
	for (size_t i = 0; i < match.size(); i++) {
		double dist = match[i].distance;
		if (dist < min_dist) min_dist = dist;
		if (dist > max_dist) max_dist = dist;
	}
	// if (debug) cout << "-- Max dist : " << max_dist << endl;
	// if (debug) cout << "-- Min dist : " << min_dist << endl;

	vector<DMatch> good_match;
	for (size_t i = 0; i < match.size(); i++) {
		if (match[i].distance < 3 * min_dist)
			good_match.push_back(match[i]);
	}
	match = good_match;
}
float GeometryAligner::getDistance(cv::Point2f p1, cv::Point2f& p2) {
	return sqrt( (p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y) );
}
bool GeometryAligner::isRightTriAngle(std::vector<cv::Point2f>& corner) {
	if (corner.size() != 3) return false;

	float l1 = getDistance(corner[0], corner[1]);
	float l2 = getDistance(corner[1], corner[2]);
	float l3 = getDistance(corner[0], corner[2]);

	// cout << l1 << " " << l2 << " " << l3 << endl;
	// cout << sqrt(l1*l1 + l2*l2) << endl;

	float err = abs(sqrt(l1 * l1 + l2 * l2) - l3);
	// cout << err << endl;

	return (err < l3 / 10);
}

bool GeometryAligner::isRectangle(std::vector<cv::Point2f>& corner) {
	if (corner.size() != 4) return false;

	vector<Point2f> tri;
	tri.push_back(corner[0]);
	tri.push_back(corner[1]);
	tri.push_back(corner[2]);
	if (!isRightTriAngle(tri)) return false;

	tri.clear();
	tri.push_back(corner[1]);
	tri.push_back(corner[2]);
	tri.push_back(corner[3]);
	if (!isRightTriAngle(tri)) return false;

	tri.clear();
	tri.push_back(corner[2]);
	tri.push_back(corner[3]);
	tri.push_back(corner[0]);
	if (!isRightTriAngle(tri)) return false;

	return true;
}
bool GeometryAligner::imageMatchSurf(Mat frame, Mat scene, Mat& H, int thresh)
{
	vector<KeyPoint> keypoint_frame, keypoint_scene;
	// if (debug) showImage("frame", frame);
	// if (debug) showImage("scene", scene);
	// if (debug) waitKey(0);

	//-- Step 1: Detect the keypoints using SURF Detector
	{
		SurfFeatureDetector detector(400);
		detector.detect(frame, keypoint_frame);
		detector.detect(scene, keypoint_scene);
		if (keypoint_frame.empty() || keypoint_scene.empty())
			return false;
	}

	//-- Step 2: Calculate descriptors (feature vectors)
	Mat descriptors_frame, descriptors_scene;
	{
		SurfDescriptorExtractor extractor;
		extractor.compute(frame, keypoint_frame, descriptors_frame);
		extractor.compute(scene, keypoint_scene, descriptors_scene);
	}

	//-- Step 3: Matching descriptor vectors using FLANN matcher
	vector<DMatch> match;
	{
		FlannBasedMatcher matcher;
		matcher.match(descriptors_frame, descriptors_scene, match);
	}

	selectGoodMatch(match);
	if (debug) cout << match.size() << endl;

	vector<Point2f> matchpoint_frame, matchpoint_scene;
	for (size_t i = 0; i < match.size(); i++) {
		matchpoint_frame.push_back(keypoint_frame[match[i].queryIdx].pt);
		matchpoint_scene.push_back(keypoint_scene[match[i].trainIdx].pt);
	}

	//-- Step 6: Localize the object from img_1 in img_2
	H = findHomography(matchpoint_frame, matchpoint_scene, RANSAC, thresh);
	// cout << H << endl;
	// Mat scene1 = scene.clone();
	// warpPerspective(frame, scene1, H, scene1.size());
	// for (int r = 0; r < scene.rows; ++r) {
	// 	for (int c = 0; c < scene.cols; ++c) {
	// 		if (scene1.at<Vec3b>(r, c) != Vec3b(0, 0, 0)) {
	// 			scene.at<Vec3b>(r, c) = scene1.at<Vec3b>(r, c);
	// 		}
	// 	}
	// }
	// showImage("scene1", scene1);
	// showImage("scene", scene);
	// waitKey(0);

	vector<Point2f> corner = getCornerOnScene(frame.size(), H);
	// for (size_t i = 0; i < corner.size(); ++i) {
	// 	circle(scene, corner[i], 10, Scalar(0, 0, 255), -1);
	// }
	// showImage("scene", scene);
	// waitKey(0);

	if (debug) writeMatchResult(frame, scene, keypoint_frame, keypoint_scene, match, H, "../match_result_1.jpg");

	if (!isRectangle(corner)) return false;
	return true;;
}
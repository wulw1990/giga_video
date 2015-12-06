#include "GeometryAligner.h"
#include <fstream>
#include <time.h>

static const int debug = 1;

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
Mat GeometryAligner::getMatOnScene(Mat frame, Mat scene, Mat H)
{
	vector<Point2f> corner_frame(4);
	corner_frame[0] = Point2f(0, 0);
	corner_frame[1] = Point2f(frame.cols, 0);
	corner_frame[2] = Point2f(frame.cols, frame.rows);
	corner_frame[3] = Point2f(0, frame.rows);

	vector<Point2f> corner_on_scene;
	perspectiveTransform(corner_frame, corner_on_scene, H);

	int x = (int)corner_on_scene[0].x;
	int y = (int)corner_on_scene[0].y;
	int w = (int)(norm(Mat(corner_on_scene[0] - corner_on_scene[1]), 2));
	int h = (int)(norm(Mat(corner_on_scene[0] - corner_on_scene[3]), 2));
	Rect rect(x, y, w, h);

	return scene(rect).clone();
}
bool GeometryAligner::isGoodMatch(Mat frame, Mat scene, int thresh)
{
	// vector<KeyPoint> keypoint_frame, keypoint_scene;
	// vector<DMatch> good_match;
	// if (!getKeyPointAndGoodMatch(frame, scene, keypoint_frame, keypoint_scene, good_match))
	// 	return false;

	// vector<Point2f> goodpoint_frame, goodpoint_scene;
	// getGoodPoint(keypoint_frame, keypoint_scene, good_match, goodpoint_frame, goodpoint_scene);

	// //-- Step 6: Localize the object from img_1 in img_2

	// Mat H = findHomography(goodpoint_frame, goodpoint_scene, RANSAC, thresh);
	// if (debug) writeMatchResult(frame, scene, keypoint_frame, keypoint_scene, good_match, H, "../match_result_2.jpg");


	// // -- succeed or not ? -----------------------------------
	// vector<Point2f> testpoint_scene(good_match.size());
	// perspectiveTransform(goodpoint_frame, testpoint_scene, H);
	// int error = 0;
	// //ofstream fout("dis.txt");
	// for (int i = 0; i < (int)good_match.size(); i++) {
	// 	Point p = goodpoint_scene[i];
	// 	Point q = testpoint_scene[i];
	// 	double dis = norm(Mat(p - q), 2);
	// 	if (debug) cout << "dis : " << dis << endl;
	// 	if (dis > thresh) error++;
	// 	//fout << dis << endl;
	// }
	// //fout.close();
	// if (debug) cout << "error : " << error << "\t" << (int)good_match.size() << endl;
	// cout << "GeometryAligner::imageMatchSurf matched" << endl;

	// return (error < (int)good_match.size() / 2);
	return true;
}
void GeometryAligner::selectGoodMatch(vector<DMatch>& match) {
	double max_dist = match[0].distance;
	double min_dist = match[0].distance;
	for (size_t i = 0; i < match.size(); i++) {
		double dist = match[i].distance;
		if (dist < min_dist) min_dist = dist;
		if (dist > max_dist) max_dist = dist;
	}
	if (debug) cout << "-- Max dist : " << max_dist << endl;
	if (debug) cout << "-- Min dist : " << min_dist << endl;

	vector<DMatch> good_match;
	for (size_t i = 0; i < match.size(); i++) {
		if (match[i].distance < 3 * min_dist)
			good_match.push_back(match[i]);
	}
	match = good_match;
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
	cout << match.size() << endl;

	vector<Point2f> goodpoint_frame, goodpoint_scene;
	for (size_t i = 0; i < match.size(); i++) {
		goodpoint_frame.push_back(keypoint_frame[match[i].queryIdx].pt);
		goodpoint_scene.push_back(keypoint_scene[match[i].trainIdx].pt);
	}

	//-- Step 6: Localize the object from img_1 in img_2
	H = findHomography(goodpoint_frame, goodpoint_scene, RANSAC, thresh);
	// cout << H << endl;


	// Mat H1 = findHomography(goodpoint_scene, goodpoint_frame, RANSAC, thresh);
	// Mat T1 = H1(Rect(0, 0, 3, 2)).clone();
	// cout << T1 << endl;
	// Mat scene1 = scene.clone();
	// warpAffine(frame, scene1, T1, scene1.size());
	// showImage("scene1", scene1);
	// showImage("scene", scene);
	// waitKey(0);


	if (debug) writeMatchResult(frame, scene, keypoint_frame, keypoint_scene, match, H, "../match_result_1.jpg");

	Mat mat_on_scene = getMatOnScene(frame, scene, H);
	//showImage("mat_on_scene", mat_on_scene);
	//showImage("frame", frame);
	//waitKey(0);

	return isGoodMatch(frame, mat_on_scene, thresh);
}
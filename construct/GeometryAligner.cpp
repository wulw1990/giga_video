#include "GeometryAligner.h"
#include <fstream>
#include <time.h>

static const int debug = 0;

bool GeometryAligner::align(Mat _frame, Mat _scene, Mat& dst_frame, Rect& dst_rect, Mat& T)
{
	const int max_width = 500;

	// -- down scale ---
	int scale = 1;
	Mat frame = _frame.clone();
	Mat scene = _scene.clone();
	while (frame.cols > max_width){
		resize(frame, frame, Size(frame.cols / 2, frame.rows / 2));
		resize(scene, scene, Size(scene.cols / 2, scene.rows / 2));
		scale *= 2;
	}

	Mat H;
	if (!imageMatchSurf(frame, scene, H, 15))
		return false;

	const int border = static_cast<int>(0.1*frame.cols);
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
	for (size_t i = 0; i < obj_corners.size(); ++i){
		obj_corners[i] *= scale;
		scene_corners[i] *= scale;
	}

	int x = (int)scene_corners[0].x;
	int y = (int)scene_corners[0].y;
	int w = (int)norm((Mat)scene_corners[0], (Mat)scene_corners[1], 2);
	int h = (int)norm((Mat)scene_corners[0], (Mat)scene_corners[3], 2);
	dst_rect = Rect(x, y, w, h);

	Rect rect_max(0, 0, _scene.cols, _scene.rows);
	if ((dst_rect&rect_max) != dst_rect)
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

bool GeometryAligner::getKeyPointAndGoodMatch(
	Mat frame, Mat scene,
	vector<KeyPoint>& keypoints_frame, 
	vector<KeyPoint>& keypoints_scene,
	vector< DMatch >& good_match)
{
	if (debug) showImage("frame", frame);
	if (debug) showImage("scene", scene);
	if (debug) waitKey(0);

	//-- Step 1: Detect the keypoints using SURF Detector   
	keypoints_frame.clear(); keypoints_scene.clear();
	{
		SurfFeatureDetector detector(400);
		detector.detect(frame, keypoints_frame);
		detector.detect(scene, keypoints_scene);
		if(debug) cout << "SurfFeatureDetector OK: " << keypoints_frame.size() << "\t" << keypoints_scene.size() << endl;

		if ((int)keypoints_frame.size() == 0 || (int)keypoints_scene.size() == 0)
			return false;
	}
	//-- Step 2: Calculate descriptors (feature vectors)  
	Mat descriptors_frame, descriptors_scene;
	{
		SurfDescriptorExtractor extractor;
		extractor.compute(frame, keypoints_frame, descriptors_frame);
		extractor.compute(scene, keypoints_scene, descriptors_scene);
		/*for (int i = 0; i < (int)keypoints_scene.size(); ++i)
			circle(scene, keypoints_scene[i].pt, 5, Scalar(0, 0, 255), -1);
		showImage("keypoints_scene", scene);
		waitKey(0);*/
		if (debug) cout << "SurfDescriptorExtractor OK: " << descriptors_frame.size() << "\t" << descriptors_scene.size() << endl;
	}
	//-- Step 3: Matching descriptor vectors using FLANN matcher   
	vector<DMatch> matches;
	{
		FlannBasedMatcher matcher;
		matcher.match(descriptors_frame, descriptors_scene, matches);
	}

	//-- Step 4: Quick calculation of max and min distances between keypoints   
	double max_dist = 0; double min_dist = 1e10;
	for (int i = 0; i < descriptors_frame.rows; i++){
		double dist = matches[i].distance;
		if (dist < min_dist) min_dist = dist;
		if (dist > max_dist) max_dist = dist;
	}
	if (debug) cout << "-- Max dist : " << max_dist << endl;;
	if (debug) cout << "-- Min dist : " << min_dist << endl;;

	//-- Step 5: Draw only "good" matches (i.e. whose distance is less than 3*min_dist )   
	good_match.resize(0);
	for (int i = 0; i < descriptors_frame.rows; i++){
		if (matches[i].distance < 3 * min_dist)
			good_match.push_back(matches[i]);
	}

	return true;
}
void GeometryAligner::getGoodPoint(
	vector<KeyPoint>& keypoints_frame,
	vector<KeyPoint>& keypoints_scene,
	vector<DMatch>& good_match,
	vector<Point2f>& goodpoint_frame,
	vector<Point2f>& goodpoint_scene)
{
	goodpoint_frame.clear();
	goodpoint_scene.clear();
	for (int i = 0; i < (int)good_match.size(); i++)
	{
		//-- Get the keypoints from the good matches   
		goodpoint_frame.push_back(keypoints_frame[good_match[i].queryIdx].pt);
		goodpoint_scene.push_back(keypoints_scene[good_match[i].trainIdx].pt);
	}
}
void GeometryAligner::writeMatchResult(
	Mat frame, Mat scene,
	vector<KeyPoint>& keypoint_frame,
	vector<KeyPoint>& keypoint_scene,
	vector<DMatch>& good_match,
	Mat H,
	string name
	)
{
	if(debug) cout << "output the match result image" << endl;
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
	line(img_matches, scene_corners[0] + offset, scene_corners[1] + offset, Scalar(0, 255, 0), 2);
	line(img_matches, scene_corners[1] + offset, scene_corners[2] + offset, Scalar(0, 255, 0), 2);
	line(img_matches, scene_corners[2] + offset, scene_corners[3] + offset, Scalar(0, 255, 0), 2);
	line(img_matches, scene_corners[3] + offset, scene_corners[0] + offset, Scalar(0, 255, 0), 2);

	imwrite(name, img_matches);
}
void GeometryAligner::drawRect(Mat img, vector<Point2f>& corner)
{
	assert(corner.size() == 4);
	line(img, corner[0], corner[1], Scalar(0, 255, 0), 2);
	line(img, corner[1], corner[2], Scalar(0, 255, 0), 2);
	line(img, corner[2], corner[3], Scalar(0, 255, 0), 2);
	line(img, corner[3], corner[0], Scalar(0, 255, 0), 2);
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

	//drawRect(frame, corner_frame);
	//drawRect(scene, corner_on_scene);
	//showImage("frame corner", frame);
	//showImage("scene corner", scene);
	//waitKey(0);

	int x = (int)corner_on_scene[0].x;
	int y = (int)corner_on_scene[0].y;
	int w = (int)(norm(Mat(corner_on_scene[0] - corner_on_scene[1]), 2));
	int h = (int)(norm(Mat(corner_on_scene[0] - corner_on_scene[3]), 2));
	Rect rect(x, y, w, h);

	return scene(rect).clone();
}
bool GeometryAligner::isGoodMatch(Mat frame, Mat scene, int thresh)
{
	vector<KeyPoint> keypoint_frame, keypoint_scene;
	vector<DMatch> good_match;
	if(!getKeyPointAndGoodMatch(frame, scene, keypoint_frame, keypoint_scene, good_match))
		return false;

	vector<Point2f> goodpoint_frame, goodpoint_scene;
	getGoodPoint(keypoint_frame, keypoint_scene, good_match, goodpoint_frame, goodpoint_scene);

	//-- Step 6: Localize the object from img_1 in img_2   

	Mat H = findHomography(goodpoint_frame, goodpoint_scene, RANSAC, thresh);
	if (debug) writeMatchResult(frame, scene, keypoint_frame, keypoint_scene, good_match, H, "match_result_2.jpg");


	// -- succeed or not ? -----------------------------------
	vector<Point2f> testpoint_scene(good_match.size());
	perspectiveTransform(goodpoint_frame, testpoint_scene, H);
	int error = 0;
	//ofstream fout("dis.txt");
	for (int i = 0; i< (int)good_match.size(); i++){
		Point p = goodpoint_scene[i];
		Point q = testpoint_scene[i];
		double dis = norm(Mat(p - q), 2);
		if (debug) cout << "dis : " << dis << endl;
		if (dis> thresh) error++;
		//fout << dis << endl;
	}
	//fout.close();
	if (debug) cout << "error : " << error << "\t" << (int)good_match.size() << endl;
	cout << "GeometryAligner::imageMatchSurf matched" << endl;
	if (debug) return (error < (int)good_match.size() / 2);

	return true;
}
bool GeometryAligner::imageMatchSurf(Mat frame, Mat scene, Mat& H, int thresh)
{
	if (debug) showImage("frame", frame);
	if (debug) showImage("scene", scene);
	if (debug) waitKey(0);

	vector<KeyPoint> keypoint_frame, keypoint_scene;
	vector<DMatch> good_match;
	if(!getKeyPointAndGoodMatch(frame, scene, keypoint_frame, keypoint_scene, good_match))
		return false;

	vector<Point2f> goodpoint_frame, goodpoint_scene;
	getGoodPoint(keypoint_frame, keypoint_scene, good_match, goodpoint_frame, goodpoint_scene);

	//-- Step 6: Localize the object from img_1 in img_2   

	H = findHomography(goodpoint_frame, goodpoint_scene, RANSAC, thresh);
	if (debug) writeMatchResult(frame, scene, keypoint_frame, keypoint_scene, good_match, H, "match_result_1.jpg");

	Mat mat_on_scene = getMatOnScene(frame, scene, H);
	//showImage("mat_on_scene", mat_on_scene);
	//showImage("frame", frame);
	//waitKey(0);

	return isGoodMatch(frame, mat_on_scene, thresh);
}
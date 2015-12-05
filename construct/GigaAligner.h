#ifndef _GIGA_ALIGNER_H_
#define _GIGA_ALIGNER_H_

#include <opencv2/opencv.hpp>
#include <string>
using namespace std;
using namespace cv;

class OpticAligner;
class GeometryAligner;

class GigaAligner
{
public:
	GigaAligner();
	~GigaAligner();
	bool alignStaticVideo(string path_scene, string input_video, string output_prefix);
	//TODO
	//void manualSelect(string path_scene, string input_video);

private:
	OpticAligner* m_optic_aligner;
	GeometryAligner* m_geometry_aligner;

	bool alignFrameToScene(string path_scene, Mat frame, Mat& dst_frame, Rect& dst_rect, Mat& T, Mat& model_hist);
	Mat readFirstFrame(string name);

	int writeStaticVideo(string name_input, string name_output, Mat T, Mat hist, Size size);

	bool writeStaticInfo(string name, int n, Rect rect);
	bool writeDynamicInfo(string name, int n, vector<Rect> rect);

	void showImage(string win_name, Mat img);
};

#endif
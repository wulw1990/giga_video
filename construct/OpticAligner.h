#ifndef _OPTIC_ALIGNER_H_
#define _OPTIC_ALIGNER_H_

#include <opencv2/opencv.hpp>
//cv::Mat HistogramMatch(const cv::Mat _src, const cv::Mat _dst );

class OpticAligner
{
public:
	cv::Mat getMatchRGBImg(const cv::Mat _origMat, const cv::Mat _modelMat);

private:

	void HistMatch(IplImage *histimg, double histv[]);
	void MyHistMatch(IplImage& img_object, IplImage* img_model);
	IplImage* getMatchRGBImg(IplImage* origImg, IplImage* modelImg);

};
#endif
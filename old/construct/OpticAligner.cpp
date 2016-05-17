#include "OpticAligner.hpp"

using namespace std;
using namespace cv;

#define cvQueryHistValue_1D( hist, idx0 ) \
    ((float)cvGetReal1D( (hist)->bins, (idx0)))//?????????????????


// the defination is designed for histgram 
#define  BINWIDTH 5
#define  HistSize 256

// the heigth and width of the histgram and the interval stand for the hist image interval
int height = 440;
int interval = cvRound(BINWIDTH * 2 / 5);
int width = HistSize * BINWIDTH + (HistSize - 1)*interval;

// do the hist match histv[] is the vector that you need match
void OpticAligner::HistMatch(IplImage *histimg, double histv[])
{
	int bins = 256;
	int sizes[] = { bins };
	CvHistogram *hist = cvCreateHist(1, sizes, CV_HIST_ARRAY);
	cvCalcHist(&histimg, hist);

	cvNormalizeHist(hist, 1);

	double val_1 = 0.0;
	double val_2 = 0.0;
	uchar T[256] = { 0 };
	double S[256] = { 0 };
	double G[256] = { 0 };

	for (int index = 0; index<256; ++index)
	{
		val_1 += cvQueryHistValue_1D(hist, index);
		val_2 += histv[index];

		G[index] = val_2;
		S[index] = val_1;
	}

	double min_val = 0.0;
	int PG = 0;

	for (int i = 0; i<256; ++i)
	{
		min_val = 1.0;
		for (int j = 0; j<256; ++j)
		{
			if ((G[j] - S[i]) < min_val && (G[j] - S[i]) >= 0)
			{
				min_val = (G[j] - S[i]);
				PG = j;
			}

		}
		T[i] = (uchar)PG;
	}

	uchar *p = NULL;
	for (int x = 0; x<histimg->height; ++x)
	{
		p = (uchar*)(histimg->imageData + histimg->widthStep*x);
		for (int y = 0; y<histimg->width; ++y)
		{
			p[y] = T[p[y]];
		}

	}


}
void OpticAligner::MyHistMatch(IplImage& img_object, IplImage* img_model)
{
	double model[256] = { 0 };

	//求模板图像的直方图，使用该直方图制作直方图模板
	int bins_model = 256;
	int sizes_model[] = { bins_model };
	CvHistogram *hist_model = cvCreateHist(1, sizes_model, CV_HIST_ARRAY);
	cvCalcHist(&img_model, hist_model);
	cvNormalizeHist(hist_model, 1);

	for (int i = 0; i<255; ++i)
	{
		model[i] = (double)cvQueryHistValue_1D(hist_model, i);
	}
	//直方图匹配
	HistMatch(&img_object, model);
}
IplImage* OpticAligner::getMatchRGBImg(IplImage* origImg, IplImage* modelImg)
{
	//分解原图
	IplImage* redImage_obj = cvCreateImage(cvGetSize(origImg), origImg->depth, 1);
	IplImage* greenImage_obj = cvCreateImage(cvGetSize(origImg), origImg->depth, 1);
	IplImage* blueImage_obj = cvCreateImage(cvGetSize(origImg), origImg->depth, 1);
	cvSplit(origImg, blueImage_obj, greenImage_obj, redImage_obj, NULL);

	//分解匹配图
	IplImage* redImage_model = cvCreateImage(cvGetSize(modelImg), modelImg->depth, 1);
	IplImage* greenImage_model = cvCreateImage(cvGetSize(modelImg), modelImg->depth, 1);
	IplImage* blueImage_model = cvCreateImage(cvGetSize(modelImg), modelImg->depth, 1);
	cvSplit(modelImg, blueImage_model, greenImage_model, redImage_model, NULL);

	//三通道分别进行匹配
	MyHistMatch(*redImage_obj, redImage_model);
	MyHistMatch(*greenImage_obj, greenImage_model);
	MyHistMatch(*blueImage_obj, blueImage_model);

	//混合三通道生成匹配图
	IplImage *histimg_object = cvCreateImage(cvGetSize(origImg), origImg->depth, 3);
	cvMerge(blueImage_obj, greenImage_obj, redImage_obj, 0, histimg_object);

	return histimg_object;
}
cv::Mat OpticAligner::getMatchRGBImg(const cv::Mat _origMat, const cv::Mat _modelMat)
{
	IplImage origIpl(_origMat);
	IplImage modelIpl(_modelMat);
	IplImage* p_origIpl = &origIpl;
	IplImage* p_modelIpl = &modelIpl;

	IplImage * p_matchImg = getMatchRGBImg(p_origIpl, p_modelIpl);
	cv::Mat matchMat(p_matchImg);
	return matchMat;
}
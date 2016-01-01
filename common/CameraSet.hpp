#ifndef CameraSet_HPP
#define CameraSet_HPP

#include <FlyCapture2.h>
#include <iostream>
#include <sstream>
#include <opencv2/opencv.hpp>

using namespace FlyCapture2;
using namespace std;
using namespace cv;

class CameraSet
{
public:
	 CameraSet();
	~CameraSet();
	ErrorType getCapture(Mat & getImage, int index);
	unsigned int getSerialNum(unsigned int index);
	void PrintBuildInfo();
	void PrintCameraInfo( CameraInfo* pCamInfo );
	void PrintError(Error error);

	unsigned int numCameras;
	Camera** ppCameras;
};

#endif
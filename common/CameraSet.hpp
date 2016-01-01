#ifndef CameraSet_HPP_
#define CameraSet_HPP_

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

		int getNumCamera() {
			return numCameras;
		}

	private:
		unsigned int numCameras;
		Camera** ppCameras;

		void setup();
		void release();
};

#endif
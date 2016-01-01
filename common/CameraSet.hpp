#ifndef CameraSet_HPP_
#define CameraSet_HPP_

#include "CameraBase.hpp"

#ifdef FLY_CAPTRUE
#include <FlyCapture2.h>

class CameraSet: public CameraBase
{
	public:
		CameraSet();
		~CameraSet();
		bool read(cv::Mat& frame, int index);
		int getNumCamera() {
			return numCameras;
		}
	private:
		unsigned int numCameras;
		FlyCapture2::Camera** ppCameras;

		void setup();
		void release();

		unsigned int getSerialNum(unsigned int index);
		void PrintBuildInfo();
		void PrintCameraInfo( FlyCapture2::CameraInfo* pCamInfo );
		void PrintError(FlyCapture2::Error error);

		FlyCapture2::ErrorType getCapture(cv::Mat & getImage, int index);
};
#else
class CameraSet: public CameraBase
{
public:
	bool read(cv::Mat& frame, int index){return false;}
	int getNumCamera() {return 0;}
};
#endif


#endif
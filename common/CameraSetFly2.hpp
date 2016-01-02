#ifndef CameraSetFly2_HPP_
#define CameraSetFly2_HPP_

#include "CameraSetBase.hpp"

// #define FLY_CAPTRUE 

#ifdef FLY_CAPTRUE
#include <FlyCapture2.h>

class CameraSetFly2: public CameraSetBase
{
	public:
		CameraSetFly2();
		~CameraSetFly2();
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
#include <iostream>
class CameraSetFly2: public CameraSetBase
{
public:
	bool read(cv::Mat& frame, int index){
		std::cerr << "no fly capture" << std::endl;
		return false;
	}
	int getNumCamera() {
		std::cerr << "no fly capture" << std::endl;
		return 0;
	}
};
#endif


#endif
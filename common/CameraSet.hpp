#ifndef CameraSet_HPP_
#define CameraSet_HPP_

#include <FlyCapture2.h>
#include "CameraBase.hpp"

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

#endif
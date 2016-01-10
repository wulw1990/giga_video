#include "CameraSetFly2.hpp"

#ifdef FLY_CAPTRUE

#include <iostream>
#include <sstream>

using namespace FlyCapture2;
using namespace std;
using namespace cv;

void CameraSetFly2::PrintBuildInfo() {
	FC2Version fc2Version;
	Utilities::GetLibraryVersion( &fc2Version );

	ostringstream version;
	version << "FlyCapture2 library version: " << fc2Version.major << "." << fc2Version.minor << "." << fc2Version.type << "." << fc2Version.build;
	cout << version.str() << endl;

	ostringstream timeStamp;
	timeStamp << "Application build date: " << __DATE__ << " " << __TIME__;
	cout << timeStamp.str() << endl << endl;
}
void CameraSetFly2::PrintCameraInfo( CameraInfo* pCamInfo ) {
	cout << endl;
	cout << "*** CAMERA INFORMATION ***" << endl;
	cout << "Serial number -" << pCamInfo->serialNumber << endl;
	cout << "Camera model - " << pCamInfo->modelName << endl;
	cout << "Camera vendor - " << pCamInfo->vendorName << endl;
	cout << "Sensor - " << pCamInfo->sensorInfo << endl;
	cout << "Resolution - " << pCamInfo->sensorResolution << endl;
	cout << "Firmware version - " << pCamInfo->firmwareVersion << endl;
	cout << "Firmware build time - " << pCamInfo->firmwareBuildTime << endl << endl;
}
void CameraSetFly2::PrintError( Error error ) {
	error.PrintErrorTrace();
}
void CameraSetFly2::setup(double shutter) {
	Error error;
	BusManager busMgr;

	error = busMgr.GetNumOfCameras(&numCameras);
	if (error != PGRERROR_OK) {
		PrintError( error );
		return;
	}
	if ( numCameras < 1 ) {
		cout << "Insufficient number of cameras... press Enter to exit." << endl; ;
		cin.ignore();
		return;
	}
	m_camera.resize(numCameras);
	for (unsigned int i = 0; i < numCameras; i ++) {
		m_camera[i] = make_shared<Camera>();

		PGRGuid guid;
		error = busMgr.GetCameraFromIndex( i, &guid );
		if (error != PGRERROR_OK) {
			PrintError( error );
			return;
		}
		error = m_camera[i]->Connect(&guid);
		if (error != PGRERROR_OK) {
			PrintError( error );
			return;
		}
		CameraInfo camInfo;
		error = m_camera[i]->GetCameraInfo( &camInfo );
		if (error != PGRERROR_OK) {
			PrintError( error );
			return;
		}
		// PrintCameraInfo(&camInfo);
		if (shutter > 0) {
			cout << "setting shutter... " << shutter << endl;
			//Declare a Property struct.
			Property prop;
			//Define the property to adjust.
			prop.type = SHUTTER;
			//Ensure the property is on.
			prop.onOff = true;
			//Ensure auto-adjust mode is off.
			prop.autoManualMode = false;
			//Ensure the property is set up to use absolute value control.
			prop.absControl = true;
			//Set the absolute value of shutter to 20 ms.
			prop.absValue = shutter;
			//Set the property.
			error = m_camera[i]->SetProperty( &prop  );
			// cout << "error: " << error << endl;
		}
		error = m_camera[i]->StartCapture();
		if ( error == PGRERROR_ISOCH_BANDWIDTH_EXCEEDED ) {
			std::cout << "Bandwidth exceeded" << std::endl;
			return;
		}
		else if ( error != PGRERROR_OK ) {
			std::cout << "Failed to start image capture" << std::endl;
			return;
		}
	}
}
CameraSetFly2::CameraSetFly2(double shutter) {
	cout << "init camera set...  " << endl;
	setup(shutter);
	release();
	setup(shutter);
	cout << "ok. n_cameras = " << numCameras << endl;
}
bool CameraSetFly2::read(cv::Mat& frame, int index) {
	ErrorType error = getCapture(frame, index);
	return (error == PGRERROR_OK);
}
ErrorType CameraSetFly2::getCapture(Mat &getImage, int index) {
	Image rawImage;
	Error error = m_camera[index]->RetrieveBuffer( &rawImage );
	if ( error != PGRERROR_OK ) {
		std::cout << "capture error" << std::endl;
		return PGRERROR_FAILED;
	}
	Image rgbImage;
	rawImage.Convert( FlyCapture2::PIXEL_FORMAT_BGR, &rgbImage );
	// convert to OpenCV Mat
	unsigned int rowBytes = (double)rgbImage.GetReceivedDataSize() / (double)rgbImage.GetRows();
	Mat(rgbImage.GetRows(), rgbImage.GetCols(), CV_8UC3, rgbImage.GetData(), rowBytes).copyTo(getImage);
	return PGRERROR_OK;
}
unsigned int CameraSetFly2::getSerialNum(unsigned int index) {
	BusManager busMgr;
	unsigned int num;
	busMgr.GetCameraSerialNumberFromIndex(index, &num);
	return num;
}
void CameraSetFly2::release() {
	for ( unsigned int i = 0; i < numCameras; i++ ) {
		m_camera[i]->StopCapture();
		m_camera[i]->Disconnect();
	}
}
CameraSetFly2::~CameraSetFly2() {
	release();
}
#endif
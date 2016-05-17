#include "CameraSetFly2.hpp"

#ifdef ENABLE_FLY_CAPTRUE

#include <iostream>
#include <fstream>
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
void CameraSetFly2::setup() {
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
		// cout << "guid: " << guid << endl;
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
		error = m_camera[i]->StartCapture();
		if ( error == PGRERROR_ISOCH_BANDWIDTH_EXCEEDED ) {
			std::cout << "Bandwidth exceeded" << std::endl;
			return;
		}
		else if ( error != PGRERROR_OK ) {
			std::cout << "Failed to start image capture" << std::endl;
			return;
		}
		// PrintCameraInfo(&camInfo);
	}
}
CameraSetFly2::CameraSetFly2(string setting_file): m_setting(setting_file) {
	cout << "init camera set...  " << endl;
	setup();
	release();
	setup();
	setStaticProperty();
	m_frame_id.resize(numCameras);
	for (size_t i = 0; i < m_frame_id.size(); ++i) {
		m_frame_id[i] = 0;
	}
	cout << "ok. n_cameras = " << numCameras << endl;
}
void CameraSetFly2::setStaticProperty() {
	for (size_t i = 0; i < m_camera.size(); ++i) {
		Property prop;
		prop.type = AUTO_EXPOSURE;
		prop.onOff = true;
		prop.autoManualMode = false;
		prop.absControl = true;
		prop.absValue = m_setting.exposure;
		Error error = m_camera[i]->SetProperty( &prop  );
	}
	for (size_t i = 0; i < m_camera.size(); ++i) {
		Property prop;
		prop.type = GAIN;
		prop.onOff = true;
		prop.autoManualMode = false;
		prop.absControl = true;
		prop.absValue = m_setting.gain;
		Error error = m_camera[i]->SetProperty( &prop  );
	}
	for (size_t i = 0; i < m_camera.size(); ++i) {
		Property prop;
		prop.type = BRIGHTNESS;
		prop.onOff = true;
		prop.autoManualMode = false;
		prop.absControl = true;
		prop.absValue = m_setting.brightness;
		Error error = m_camera[i]->SetProperty( &prop  );
	}
}
bool CameraSetFly2::read(cv::Mat& frame, int index) {
	if (index < 0 || index > (int)numCameras) {
		return false;
	}
	// cout << "m_frame_id " << index << " : " << m_frame_id[index] << endl;
	m_frame_id[index]++;
	if (index < (int)m_setting.shutter_dynamic.size() ) {
		double shutter = m_setting.shutter_min[index];
		if (m_setting.shutter_dynamic[index]) {
			shutter = (m_setting.shutter_max[index] - m_setting.shutter_min[index]) /
		                m_setting.shutter_step[index] * m_frame_id[index];
		}
		Property prop;
		prop.type = SHUTTER;
		prop.onOff = true;
		prop.autoManualMode = false;
		prop.absControl = true;
		prop.absValue = shutter;
		Error error = m_camera[index]->SetProperty( &prop  );
	}
	if (m_frame_id[index] >= m_setting.shutter_step[index]) {
		m_frame_id[index] = 0;
	}
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
CameraSetFly2::Setting::Setting(string setting_file) {
	FileStorage fs(setting_file, FileStorage::READ);
	if (!fs.isOpened()) {
		cout << "Could not open the configuration file: \"" << setting_file << "\"" << endl;
		exit(-1);
	}
	// const int W = 20;
	cout << "-------------------Setting : ---------------------" << endl;
	exposure = atof(readStringItem(fs, "exposure").c_str());
	gain = atof(readStringItem(fs, "gain").c_str());
	brightness = atof(readStringItem(fs, "brightness").c_str());

	shutter_dynamic = stringList2IntList(readStringList(fs, "shutter_dynamic"));
	shutter_min = stringList2DoubleList(readStringList(fs, "shutter_min"));
	shutter_max = stringList2DoubleList(readStringList(fs, "shutter_max"));
	shutter_step = stringList2IntList(readStringList(fs, "shutter_step"));

	assert(shutter_dynamic.size() == shutter_min.size());
	assert(shutter_dynamic.size() == shutter_max.size());
	assert(shutter_dynamic.size() == shutter_step.size());

	fs.release();
	cout << "--------------------------------------------------------" << endl;
}
string CameraSetFly2::Setting::readStringItem(FileStorage& fs, string node_name) {
	string str;
	fs[node_name ] >> str;
	cout << node_name << " : " << str << endl;
	return str;
}
vector<string> CameraSetFly2::Setting::readStringList(FileStorage& fs, string node_name) {
	vector<string> list;
	FileNode node = fs[node_name];
	FileNodeIterator it = node.begin(), it_end = node.end();
	for ( ; it != it_end; ++it )
		list.push_back((string)*it);
	cout << node_name << " : " << list.size() << endl;
	for (size_t i = 0; i < list.size(); ++i)
		cout <<  " \t " << list[i] << endl;
	return list;
}
vector<int> CameraSetFly2::Setting::stringList2IntList(std::vector<std::string> str) {
	vector<int> int_list(str.size());
	for (size_t i = 0; i < str.size(); ++i) {
		int_list[i] = atoi(str[i].c_str());
	}
	return int_list;
}
vector<double> CameraSetFly2::Setting::stringList2DoubleList(std::vector<std::string> str) {
	vector<double> double_list(str.size());
	for (size_t i = 0; i < str.size(); ++i) {
		double_list[i] = atof(str[i].c_str());
	}
	return double_list;
}
#endif
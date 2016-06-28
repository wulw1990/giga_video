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
  Utilities::GetLibraryVersion(&fc2Version);

  ostringstream version;
  version << "FlyCapture2 library version: " << fc2Version.major << "."
          << fc2Version.minor << "." << fc2Version.type << "."
          << fc2Version.build;
  cout << version.str() << endl;

  ostringstream timeStamp;
  timeStamp << "Application build date: " << __DATE__ << " " << __TIME__;
  cout << timeStamp.str() << endl << endl;
}
void CameraSetFly2::PrintCameraInfo(CameraInfo *pCamInfo) {
  cout << endl;
  cout << "*** CAMERA INFORMATION ***" << endl;
  cout << "Serial number -" << pCamInfo->serialNumber << endl;
  cout << "Camera model - " << pCamInfo->modelName << endl;
  cout << "Camera vendor - " << pCamInfo->vendorName << endl;
  cout << "Sensor - " << pCamInfo->sensorInfo << endl;
  cout << "Resolution - " << pCamInfo->sensorResolution << endl;
  cout << "Firmware version - " << pCamInfo->firmwareVersion << endl;
  cout << "Firmware build time - " << pCamInfo->firmwareBuildTime << endl
       << endl;
}
void CameraSetFly2::PrintError(Error error) { error.PrintErrorTrace(); }
void CameraSetFly2::setup() {
  Error error;
  BusManager busMgr;

  error = busMgr.GetNumOfCameras(&numCameras);
  if (error != PGRERROR_OK) {
    PrintError(error);
    return;
  }
  if (numCameras < 1) {
    cout << "Insufficient number of cameras... press Enter to exit." << endl;
    ;
    cin.ignore();
    return;
  }
  m_camera.resize(numCameras);
  for (unsigned int i = 0; i < numCameras; i++) {
    m_camera[i] = make_shared<Camera>();

    PGRGuid guid;
    error = busMgr.GetCameraFromIndex(i, &guid);
    if (error != PGRERROR_OK) {
      PrintError(error);
      return;
    }
    // cout << "guid: " << guid << endl;
    error = m_camera[i]->Connect(&guid);
    if (error != PGRERROR_OK) {
      PrintError(error);
      return;
    }
    CameraInfo camInfo;
    error = m_camera[i]->GetCameraInfo(&camInfo);
    if (error != PGRERROR_OK) {
      PrintError(error);
      return;
    }
    error = m_camera[i]->StartCapture();
    if (error == PGRERROR_ISOCH_BANDWIDTH_EXCEEDED) {
      std::cout << "Bandwidth exceeded" << std::endl;
      return;
    } else if (error != PGRERROR_OK) {
      std::cout << "Failed to start image capture" << std::endl;
      return;
    }
    // PrintCameraInfo(&camInfo);
  }
}
CameraSetFly2::CameraSetFly2() {
  cout << "CameraSetFly2::init camera set...  " << endl;
  setup();
  release();
  setup();
  setStaticProperty();
  m_frame_id.resize(numCameras);
  for (size_t i = 0; i < m_frame_id.size(); ++i) {
    m_frame_id[i] = 0;
  }
  cout << "CameraSetFly2::init ok. n_cameras = " << numCameras << endl;
}
void CameraSetFly2::setStaticProperty() {
#if 0
  for (size_t i = 0; i < m_camera.size(); ++i) {
    Property prop;
    prop.type = AUTO_EXPOSURE;
    prop.onOff = true;
    prop.autoManualMode = false;
    prop.absControl = true;
    prop.absValue = m_setting.exposure;
    Error error = m_camera[i]->SetProperty(&prop);
  }
  for (size_t i = 0; i < m_camera.size(); ++i) {
    Property prop;
    prop.type = GAIN;
    prop.onOff = true;
    prop.autoManualMode = false;
    prop.absControl = true;
    prop.absValue = m_setting.gain;
    Error error = m_camera[i]->SetProperty(&prop);
  }
  for (size_t i = 0; i < m_camera.size(); ++i) {
    Property prop;
    prop.type = BRIGHTNESS;
    prop.onOff = true;
    // prop.autoManualMode = false;
    prop.autoManualMode = true;
    prop.absControl = true;
    prop.absValue = m_setting.brightness;
    Error error = m_camera[i]->SetProperty(&prop);
  }
  for (size_t i = 0; i < m_camera.size(); ++i) {
    Property prop;
    prop.type = SHUTTER;
    // prop.onOff = true;
    prop.autoManualMode = true;
    Error error = m_camera[i]->SetProperty(&prop);
  }
#endif
}
bool CameraSetFly2::read(cv::Mat &frame, int index) {
  if (index < 0 || index > (int)numCameras) {
    return false;
  }
  // cout << "m_frame_id " << index << " : " << m_frame_id[index] << endl;
  m_frame_id[index]++;
  ErrorType error = getCapture(frame, index);
  return (error == PGRERROR_OK);
}
ErrorType CameraSetFly2::getCapture(Mat &getImage, int index) {
  Image rawImage;
  Error error = m_camera[index]->RetrieveBuffer(&rawImage);
  if (error != PGRERROR_OK) {
    std::cout << "capture error" << std::endl;
    return PGRERROR_FAILED;
  }
  Image rgbImage;
  rawImage.Convert(FlyCapture2::PIXEL_FORMAT_BGR, &rgbImage);
  // convert to OpenCV Mat
  unsigned int rowBytes =
      (double)rgbImage.GetReceivedDataSize() / (double)rgbImage.GetRows();
  Mat(rgbImage.GetRows(), rgbImage.GetCols(), CV_8UC3, rgbImage.GetData(),
      rowBytes)
      .copyTo(getImage);
  return PGRERROR_OK;
}
unsigned int CameraSetFly2::getSerialNum(unsigned int index) {
  BusManager busMgr;
  unsigned int num;
  busMgr.GetCameraSerialNumberFromIndex(index, &num);
  return num;
}
void CameraSetFly2::release() {
  for (unsigned int i = 0; i < numCameras; i++) {
    m_camera[i]->StopCapture();
    m_camera[i]->Disconnect();
  }
}
CameraSetFly2::~CameraSetFly2() { release(); }
#endif
#ifndef CameraSetFly2_HPP_
#define CameraSetFly2_HPP_

#include "CameraSetBase.hpp"

#ifdef ENABLE_FLY_CAPTRUE
#include <FlyCapture2.h>
#include <memory>

class CameraSetFly2 : public CameraSetBase {
private:
  class Setting {
  public:
    Setting(){}
    Setting(std::string setting_file);
    double exposure;
    double gain;
    double brightness;

    std::vector<int> shutter_dynamic;
    std::vector<double> shutter_min;
    std::vector<double> shutter_max;
    std::vector<int> shutter_step;

  private:
    std::string readStringItem(cv::FileStorage &fs, std::string node_name);
    std::vector<std::string> readStringList(cv::FileStorage &fs,
                                            std::string node_name);
    std::vector<int> stringList2IntList(std::vector<std::string> str);
    std::vector<double> stringList2DoubleList(std::vector<std::string> str);
  };
  Setting m_setting;

public:
  CameraSetFly2();
  CameraSetFly2(std::string setting_file);
  ~CameraSetFly2();
  bool read(cv::Mat &frame, int index);
  int getNumCamera() {
    return numCameras;
    // return min(4, numCameras);
  }

private:
  unsigned int numCameras;
  std::vector<std::shared_ptr<FlyCapture2::Camera>> m_camera;

  void setup();
  void release();

  unsigned int getSerialNum(unsigned int index);
  void PrintBuildInfo();
  void PrintCameraInfo(FlyCapture2::CameraInfo *pCamInfo);
  void PrintError(FlyCapture2::Error error);

  FlyCapture2::ErrorType getCapture(cv::Mat &getImage, int index);

  void setStaticProperty();

  std::vector<int> m_frame_id;
};
#else
#include <iostream>
class CameraSetFly2 : public CameraSetBase {
public:
  CameraSetFly2() {}
  CameraSetFly2(std::string setting_file) {}
  bool read(cv::Mat &frame, int index) {
    printError();
    return false;
  }
  int getNumCamera() {
    printError();
    return 0;
  }
  private:
  void printError(){
    std::cerr << "#######################" << std::endl;
    std::cerr << "### FLY_CAPTRUE OFF ###" << std::endl;
    std::cerr << "#######################" << std::endl;
  }
};
#endif

#endif

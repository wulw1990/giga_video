#ifndef PYRAMID_CAMERA_BASE_HPP_
#define PYRAMID_CAMERA_BASE_HPP_

#include <opencv2/opencv.hpp>

class PyramidCameraBase {
public:
    virtual bool read(cv::Mat& frame, int layer_id) = 0;
    virtual int getNumLayer() = 0;
    
};

#endif
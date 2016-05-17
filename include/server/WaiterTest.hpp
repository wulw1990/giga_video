#ifndef WAITER_TEST_HPP_
#define WAITER_TEST_HPP_

#include "WaiterBase.hpp"

class WaiterTest: public WaiterBase{
public:
    void move(float dx, float dy);
    void zoom(float dz);
    
    bool hasFrame();
    void getFrame(cv::Mat& frame);
    
    bool hasThumbnail();
    void getThumbnail(std::vector<cv::Mat>& thumbnail);
};

#endif
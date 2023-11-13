#ifndef CAR_DETECTION_H
#define CAR_DETECTION_H

#include <fstream>
#include <sstream>
#include <iostream>

#include <opencv2/dnn.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include "car.h"

using namespace cv;
using namespace dnn;
using namespace std;

class Car_Detection
{
    public:
        Car_Detection();
        ~Car_Detection(){};
        vector<cv::Rect2d> callNetworks(Mat & frame);
        vector<String> getOutputsNames(const Net& net);
    private:
        vector<cv::Rect2d> findCars(Mat& frame, const vector<Mat>& outs, float confThreshold = 0.3);
        Net net;
};
#endif
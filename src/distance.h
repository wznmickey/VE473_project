#ifndef DISTANCE_H
#define DISTANCE_H

#include <iostream>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <opencv2/core/types.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <vector>
#include <opencv2/core/core.hpp>
using namespace cv;
using namespace std;



class DistanceCalc 
{

private:
    Mat left_camera_matrix;
    Mat left_dist_coeffs;
    Mat right_camera_matrix;
    Mat right_dist_coeffs;
    cv::Ptr<cv::StereoBM> stereo;
    cv::Mat disparityMap;
    ushort* depthData;
    short int* dispData;
    cv::Mat ans;

public:
    DistanceCalc();
    void distcalcinit(void);
    vector<Mat> seperatePhoto(cv::Mat image);
    double calculateDistance(vector<Mat> vec,cv::Rect2d roi);
    void calculateMap(vector<Mat> vec);
    double getBlockPropotion(cv::Rect2d roi);
};

#endif
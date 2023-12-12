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
    cv::Ptr< cv::StereoSGBM > sgbm;
    cv::Mat disparityMap;
    ushort* depthData;
    short int* dispData;
    cv::Mat ans;
    cv::Mat xyz;
    // cv::Mat mapLx, mapLy, mapRx, mapRy;

public:
    DistanceCalc();
    //void distcalcinit(void);
    double calculateDistance(vector<Mat> & vec,cv::Rect2i & roi);
    void calculateMap(vector<Mat> & vec, cv::Mat & Q);
    double getBlockPropotion(cv::Rect2i & roi);
    // vector<Mat> photoprogress(vector<Mat> & vec);

};

#endif

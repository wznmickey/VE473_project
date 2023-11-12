#include <iostream>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <vector>
#include <opencv2/core/core.hpp>
using namespace cv;
using namespace std;

Mat left_camera_matrix;
Mat left_dist_coeffs;
Mat right_camera_matrix;
Mat right_dist_coeffs;


vector<Mat> seperatePhoto(cv::Mat image)
{
    int width = image.cols;
    int height = image.rows;

    // 定义左半部分和右半部分的矩形区域
    cv::Rect left_rect(0, 0, width / 2, height);
    cv::Rect right_rect(width / 2, 0, width / 2, height);

    // 使用矩形区域截取左半部分和右半部分
    cv::Mat left_half = image(left_rect);
    cv::Mat right_half = image(right_rect);
    
    vector<Mat> ans;

    cv::undistort( left_half, left_half.clone(), left_camera_matrix, left_dist_coeffs);
    cv::undistort( right_half, right_half.clone(), right_camera_matrix, right_dist_coeffs);
    cv::cvtColor(left_half, left_half,COLOR_BGR2GRAY);
    cv::cvtColor(right_half, right_half,COLOR_BGR2GRAY);
    ans.push_back(left_half.clone());
    ans.push_back(right_half.clone());

    // cv::namedWindow("Left Half", cv::WINDOW_NORMAL);
    // cv::imshow("Left Half", ans[0]);

    // cv::namedWindow("Right Half", cv::WINDOW_NORMAL);
    // cv::imshow("Right Half", ans[1]);

    // cv::waitKey(0);

    return ans;
}


void calculateDistance(vector<Mat> vec)
{

    cv::Mat disparityMap;

cv::Ptr<cv::StereoBM> stereo = cv::StereoBM::create();

stereo->setBlockSize(5);        // 设置匹配块大小，尺寸必须是奇数，一般选择范围在5-21之间
stereo->setMinDisparity(0);      // 设置最小视差值，默认为0
stereo->setNumDisparities(16);   // 设置视差搜索范围，默认为16，即最大视差值为16
stereo->setPreFilterSize(5);     // 设置预处理滤波器的尺寸，默认为5
stereo->setPreFilterCap(31);     // 设置预处理滤波器的最大灰度差值，默认为31
stereo->setTextureThreshold(10); // 设置纹理阈值，默认为10，用于筛选边缘区域
stereo->setUniquenessRatio(15);  // 设置唯一性比率，默认为15，用于筛选匹配的唯一性

vec[0].convertTo(vec[0], CV_8UC1);

vec[1].convertTo(vec[1], CV_8UC1);


stereo->compute(vec[0], vec[1], disparityMap);

Mat ans = disparityMap.clone();
int type = disparityMap.type();
ans.convertTo(ans, CV_16UC1);

cv::normalize(disparityMap, disparityMap,0,255,NORM_MINMAX,CV_8U);


    cv::imshow("a", disparityMap);
    cv::waitKey(1);
    float fx = 21;
    float baseline = 100; //基线距离650mm

    if (type == CV_8U)
    {
        const float PI = 3.14159265358;
        int height = disparityMap.rows;
        int width = disparityMap.cols;

        uchar* dispData = (uchar*)disparityMap.data;
        ushort* depthData = (ushort*)ans.data;
        for (int i = 0; i < height; i++)
        {
            for (int j = 0; j < width; j++)
            {
                int id = i*width + j;
                // cout<< i<<" " <<j<<" " <<height <<" " <<width<<endl;
                if (!dispData[id])  continue;  //防止0除
                depthData[id] = ushort( (float)fx *baseline / ((float)dispData[id]) );
                // cout<<depthData[id]<<endl;
            }
        }
    }
return ;
}



int main()
{
    cv::FileStorage left_setting("left.yml", cv::FileStorage::READ);
    cv::FileStorage right_setting("right.yml", cv::FileStorage::READ);
    left_setting["camera_matrix"] >> left_camera_matrix;
    left_setting["dist_coeffs"] >> left_dist_coeffs;
    right_setting["camera_matrix"] >> right_camera_matrix;
    right_setting["dist_coeffs"] >> right_dist_coeffs;

    left_setting.release();
    right_setting.release();
        auto video = VideoCapture(0);

    while (true)
    {
        cv::Mat image;
        video>>image;
        calculateDistance(seperatePhoto(image));
    }
}
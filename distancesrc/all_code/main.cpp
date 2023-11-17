#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <bits/stdc++.h>
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <stdio.h>
#include <vector>

using namespace cv;
using namespace std;
cv::Mat image;
Mat     left_camera_matrix;
Mat     left_dist_coeffs;
Mat     right_camera_matrix;
Mat     right_dist_coeffs;

int              setBlockSize        = 5;  // 设置匹配块大小，尺寸必须是奇数，一般选择范围在5-21之间
int              setMinDisparity     = 0;  // 设置最小视差值，默认为0
int              setNumDisparities   = 16; // 设置视差搜索范围，默认为16，即最大视差值为16
int              setPreFilterSize    = 5;  // 设置预处理滤波器的尺寸，默认为5
int              setPreFilterCap     = 31; // 设置预处理滤波器的最大灰度差值，默认为31
int              setTextureThreshold = 10; // 设置纹理阈值，默认为10，用于筛选边缘区域
int              setUniquenessRatio  = 15; // 设置唯一性比率，默认为15，用于筛选匹配的唯一性
cv::VideoCapture video;

vector< Mat > seperatePhoto( cv::Mat image )
{
    int width  = image.cols;
    int height = image.rows;

    // 定义左半部分和右半部分的矩形区域
    cv::Rect left_rect( 0, 0, width / 2, height );
    cv::Rect right_rect( width / 2, 0, width / 2, height );

    // 使用矩形区域截取左半部分和右半部分
    cv::Mat left_half  = image( left_rect );
    cv::Mat right_half = image( right_rect );

    vector< Mat > ans;

    cv::undistort( left_half, left_half.clone( ), left_camera_matrix, left_dist_coeffs );
    cv::undistort( right_half, right_half.clone( ), right_camera_matrix, right_dist_coeffs );
    // vector<Mat> channels;
    //    通道分离
    // split(left_half, channels);

    //    分别得到不同的颜色分量
    // Mat blue, green, red;
    // left_half = channels.at(0);
    // green = channels.at(1);
    // red = channels.at(2);
    // vector<Mat> channels2;
    // split(right_half, channels2);

    // right_half = channels2.at(0);

    cv::cvtColor( left_half, left_half, COLOR_BGR2GRAY );
    cv::cvtColor( right_half, right_half, COLOR_BGR2GRAY );
    ans.push_back( left_half.clone( ) );
    ans.push_back( right_half.clone( ) );

    // cv::namedWindow("Left Half", cv::WINDOW_NORMAL);
    // cv::imshow("Left Half", ans[0]);

    // cv::namedWindow("Right Half", cv::WINDOW_NORMAL);
    // cv::imshow("Right Half", ans[1]);

    // cv::waitKey(0);

    return ans;
}

void calculateDistance( vector< Mat > vec )
{
    cv::imshow( "origin", vec [ 0 ] );

    cv::Mat disparityMap;

    int                       numberOfDisparities = ( ( vec [ 0 ].cols / 8 ) + 15 ) & -16;
    cv::Ptr< cv::StereoSGBM > sgbm                = cv::StereoSGBM::create( 0, 16, 3 );
    sgbm->setPreFilterCap( 32 );
    int SADWindowSize = 9;
    int sgbmWinSize   = SADWindowSize > 0 ? SADWindowSize : 3;
    sgbm->setBlockSize( sgbmWinSize );
    int cn =3;
    sgbm->setP1( 8 * cn * sgbmWinSize * sgbmWinSize );
    sgbm->setP2( 32 * cn * sgbmWinSize * sgbmWinSize );
    sgbm->setMinDisparity( 0 );
    sgbm->setNumDisparities( numberOfDisparities );
    sgbm->setUniquenessRatio( 10 );
    sgbm->setSpeckleWindowSize( 100 );
    sgbm->setSpeckleRange( 32 );
    sgbm->setDisp12MaxDiff( 1 );
    sgbm->setMode( cv::StereoSGBM::MODE_SGBM );
    // cout << vec [ 0 ].type()<<endl;
    vec [ 0 ].convertTo( vec [ 0 ], CV_8UC1 );

    vec [ 1 ].convertTo( vec [ 1 ], CV_8UC1 );

    sgbm->compute( vec [ 0 ], vec [ 1 ], disparityMap );

    // cout << disparityMap.type()<<endl;
    Mat ans = disparityMap.clone( );
    // cout << disparityMap << endl;

    disparityMap.convertTo( disparityMap, CV_8UC1);
    //  cout<<disparityMap<<endl;
    applyColorMap( disparityMap, ans, COLORMAP_RAINBOW );
    cv::imshow( "temp", ans );
    cv::waitKey( 15 );
    // int type = disparityMap.type( );
    // ans.convertTo( ans, CV_16UC1 );

    // float fx = 21;
    // float baseline = 100; //基线距离650mm
    // double finalDistance = 0;
    // auto countedPoint = 0;
    // if (type == CV_8U)
    // {
    //     const float PI = 3.14159265358;
    //     int height = disparityMap.rows;
    //     int width = disparityMap.cols;

    //     uchar* dispData = (uchar*)disparityMap.data;
    //     ushort* depthData = (ushort*)ans.data;
    //     for (int i = 0; i < height; i++)
    //     {
    //         for (int j = 0; j < width; j++)
    //         {
    //             int id = i*width + j;
    //             // cout<< i<<" " <<j<<" " <<height <<" " <<width<<endl;
    //             if (!dispData[id])  continue;  //防止0除
    //             depthData[id] = ushort( (float)fx *baseline / ((float)dispData[id]) );

    //             // if (i>=roi.y && i<=roi.y+roi.height && j>=roi.x && j<=roi.x+roi.width)
    //             // {
    //             //     finalDistance+=depthData[id];
    //             //     countedPoint++;
    //             // }

    //             // cout<<depthData[id]<<endl;
    //         }
    //     }
    // }
    // cout<<"WE counted" <<countedPoint<<endl;
    // return finalDistance/countedPoint; //
}
void init( )
{
    namedWindow( "ABC" );
    // 在创建窗体中创建一个滑动条
    createTrackbar( "setBlockSize", "ABC", &setBlockSize, 35 );
    createTrackbar( "setMinDisparity", "ABC", &setMinDisparity, 35 );
    createTrackbar( "setNumDisparities", "ABC", &setNumDisparities, 35 );
    createTrackbar( "setPreFilterSize", "ABC", &setPreFilterSize, 35 );
    createTrackbar( "setPreFilterCap", "ABC", &setPreFilterCap, 35 );
    createTrackbar( "setTextureThreshold", "ABC", &setTextureThreshold, 35 );
    createTrackbar( "setUniquenessRatio", "ABC", &setUniquenessRatio, 35 );
    waitKey( 0 );
    return;
}

int main( )
{
    cv::FileStorage left_setting( "left.yml", cv::FileStorage::READ );
    cv::FileStorage right_setting( "right.yml", cv::FileStorage::READ );
    left_setting [ "camera_matrix" ] >> left_camera_matrix;
    left_setting [ "dist_coeffs" ] >> left_dist_coeffs;
    right_setting [ "camera_matrix" ] >> right_camera_matrix;
    right_setting [ "dist_coeffs" ] >> right_dist_coeffs;

    left_setting.release( );
    right_setting.release( );
    video = VideoCapture(
        "/home/wznmickey/SJTU/SJTU/JI/VE473-2023/project/distance/VE473_project/distancesrc/all_code/good_videos/hit_car_bottom.mp4" );
    init( );
    while ( true )
    {
        cv::Mat image;
        video >> image;
        if ( image.empty( ) )
        {
            video = VideoCapture(
                "/home/wznmickey/SJTU/SJTU/JI/VE473-2023/project/distance/VE473_project/distancesrc/all_code/good_videos/hit_car_bottom.mp4" );
        }
        try
        {
            calculateDistance( seperatePhoto( image ) );
        }
        catch ( Exception x )
        {
            cout << x.what( ) << endl;
            cv::waitKey( 15 );
            // sleep(1);
        }
    }
}
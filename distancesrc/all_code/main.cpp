#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <bits/stdc++.h>
#include <iostream>
#include <map>
#include <opencv2/core.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/core/hal/interface.h>
#include <opencv2/core/types.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <stdio.h>
#include <vector>
// #include <matplotlibcpp.h>


using namespace cv;
using namespace std;
cv::Mat image;
Mat     left_camera_matrix;
Mat     left_dist_coeffs;
Mat     right_camera_matrix;
Mat     right_dist_coeffs;

int setNumDisparities  = 3;
int setPreFilterCap    = 25;
int setUniquenessRatio = 15;
int setsgbmWinSize     = 15;
// int              setP1                = 10;
// int              setP2                = 50;
int              setSpeckleWindowSize = 40;
int              setSpeckleRange      = 69;
int              setDisp12MaxDiff     = -1;
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

    // cv::resize(left_half, left_half, cv::Size(2160,1440));
    // cv::resize(right_half, right_half, cv::Size(2160,1440));

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

    // cv::cvtColor( left_half, left_half, COLOR_BGR2GRAY );
    // cv::cvtColor( right_half, right_half, COLOR_BGR2GRAY );
    ans.push_back( left_half.clone( ) );
    ans.push_back( right_half.clone( ) );

    // cv::namedWindow("Left Half", cv::WINDOW_NORMAL);
    // cv::imshow("Left Half", ans[0]);

    // cv::namedWindow("Right Half", cv::WINDOW_NORMAL);
    // cv::imshow("Right Half", ans[1]);

    // cv::waitKey(0);

    return ans;
}
void countData1( cv::Mat x )
{
    map< short, int > ans;
    for ( int i = 0; i < x.rows; i++ )
    {
        for ( int j = 0; j < x.cols; j++ )
        {
            auto temp = x.at< short >( i, j );
            if ( ! ans.contains( temp ) )
                ans.insert( { temp, 0 } );
            ans [ temp ] += 1;
        }
    }
    int maxAns = -16;
    for ( auto i : ans )
    {
        if ( ( ( int ) i.first ) > maxAns )
            maxAns = i.first;
        // cout << (int)i.first << " " << i.second << endl;
    }
    cout << "1 maxAns" << maxAns << endl;
}

void countData2( cv::Mat x )
{
    map< uchar, int > ans;
    for ( int i = 0; i < x.rows; i++ )
    {
        for ( int j = 0; j < x.cols; j++ )
        {
            auto temp = x.at< uchar >( i, j );
            if ( ! ans.contains( temp ) )
                ans.insert( { temp, 0 } );
            ans [ temp ] += 1;
        }
    }
    int maxAns = -16;
    for ( auto i : ans )
    {
        if ( ( ( int ) i.first ) > maxAns )
            maxAns = i.first;
        // cout << (int)i.first << " " << i.second << endl;
    }
    cout << "2 maxAns" << maxAns << endl;
}
void calculateDistance( vector< Mat > vec )
{
    Mat tempp;
    // resize(vec[0], tempp, Size(480,480));
    cv::imshow( "origin", vec [ 0 ] );

    cv::Mat disparityMap;

    // int                       numberOfDisparities = ( ( vec [ 0 ].cols / 8 ) + 15 ) & -16;
    cv::Ptr< cv::StereoSGBM > sgbm = cv::StereoSGBM::create( 0, setNumDisparities );
    sgbm->setPreFilterCap( setPreFilterCap );
    // int SADWindowSize = 9;
    // int sgbmWinSize   = SADWindowSize > 0 ? SADWindowSize : 3;
    sgbm->setBlockSize( setsgbmWinSize );
    // int cn =3;
    sgbm->setP1( setsgbmWinSize * 8 );
    sgbm->setP2( setsgbmWinSize * 32 );
    sgbm->setUniquenessRatio( setUniquenessRatio );
    sgbm->setSpeckleWindowSize( setSpeckleWindowSize );
    sgbm->setSpeckleRange( setSpeckleRange );
    sgbm->setDisp12MaxDiff( setDisp12MaxDiff );
    sgbm->setMode( cv::StereoSGBM::MODE_SGBM_3WAY );
    // cout << vec [ 0 ].type()<<endl;
    // vec [ 0 ].convertTo( vec [ 0 ], CV_8UC1 );

    // vec [ 1 ].convertTo( vec [ 1 ], CV_8UC1 );

    sgbm->compute( vec [ 0 ], vec [ 1 ], disparityMap );

    // cout << disparityMap.type()<<endl;
    Mat ans = disparityMap.clone( );
    // cout << disparityMap << endl;

    countData1( ans );

    disparityMap.convertTo( disparityMap, CV_8UC1, 3,17);
    //  cout<<disparityMap<<endl;
    countData2( disparityMap );

    // applyColorMap( disparityMap, ans, COLORMAP_RAINBOW );

    cv::imshow( "temp", disparityMap );

    // resize( ans, tempp, Size( 480, 480 ) );
    // cv::imshow( "temp", ans );

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
    createTrackbar( "setNumDisparities", "ABC", &setNumDisparities, 100 );
    // createTrackbar( "setP1", "ABC", &setP1, 100 );
    // createTrackbar( "setP2", "ABC", &setP2, 100 );

    createTrackbar( "setPreFilterCap", "ABC", &setPreFilterCap, 35 );
    createTrackbar( "setsgbmWinSize", "ABC", &setsgbmWinSize, 35 );
    createTrackbar( "setUniquenessRatio", "ABC", &setUniquenessRatio, 35 );

    createTrackbar( "setSpeckleWindowSize", "ABC", &setSpeckleWindowSize, 200 );
    createTrackbar( "setSpeckleRange", "ABC", &setSpeckleRange, 200 );

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
    video
        = VideoCapture( "/home/wznmickey/SJTU/SJTU/JI/VE473-2023/project/distance/VE473_project/distancesrc/all_code/good_videos/hit_car_left.mp4" );
    init( );
    while ( true )
    {
        // cv::Mat imageleft;
        // video >> image;
        // imageleft = imread( "/home/wznmickey/SJTU/SJTU/JI/VE473-2023/project/distance/20231120-165813.jpg" );
        // cv::Mat imageright;
        // video >> image;
        // imageright = imread( "/home/wznmickey/SJTU/SJTU/JI/VE473-2023/project/distance/20231120-205433.jpg" );

        video >> image;
        // video >> image;
        // video >> image;
        // video >> image;
        if ( image.empty( ) )
        {
            video = VideoCapture(
                "/home/wznmickey/SJTU/SJTU/JI/VE473-2023/project/distance/VE473_project/distancesrc/all_code/good_videos/hit_car_left.mp4" );
        }
        // vector< Mat > x;
        // x.push_back( imageleft );
        // x.push_back( imageright );
        try
        {
            calculateDistance(
                // x
                seperatePhoto( image ) );
        }
        catch ( Exception x )
        {
            cout << x.what( ) << endl;
            cv::waitKey( 1 );
            // sleep(1);
        }
    }
}
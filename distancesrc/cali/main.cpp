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
    // 显示图像
    
    vector<Mat> ans;

    ans.push_back(left_half.clone());
    ans.push_back(right_half.clone());

    // cv::namedWindow("Left Half", cv::WINDOW_NORMAL);
    // cv::imshow("Left Half", ans[0]);

    // cv::namedWindow("Right Half", cv::WINDOW_NORMAL);
    // cv::imshow("Right Half", ans[1]);

    // cv::waitKey(0);

    return ans;
}



int main()
{

    int iCameraCounts = 1;
    int iStatus = -1;
    //IplImage *iplImage = NULL;
    int channel = 3;




    const int board_w = 10,
              board_h = 7;
    const int board_n = board_w * board_h;
    Size board_size(10, 7);
    Mat gray_img, drawn_img;
    std::vector<Point2f> point_pix_pos_buf;
    std::vector<std::vector<Point2f>> point_pix_pos;
    int found, successes = 0;
    Size img_size;
    int k = 0, n = 0;
    auto video = VideoCapture(0);
    // if (video.set(CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M', 'J', 'P', 'G')))
    // {
    //     cout<<"may success in MJPG"<<endl;
    // }
    // if (video.set(CAP_PROP_BUFFERSIZE,1))
    // {
    //     cout<<"may success in CAP_PROP_BUFFERSIZE"<<endl;
    // }
    // video.set(CAP_PROP_FRAME_WIDTH,1280);
    // video.set(CAP_PROP_FRAME_HEIGHT, 480);
    if (video.isOpened())
 {
     cout << "视频中图像的宽度=" << video.get(CAP_PROP_FRAME_WIDTH) << endl;
     cout << "视频中图像的高度=" << video.get(CAP_PROP_FRAME_HEIGHT) << endl;
    //  cout << "视频帧率=" << video.get(CAP_PROP_FPS) << endl;
    //  cout << "视频的总帧数=" << video.get(CAP_PROP_FRAME_COUNT);
 }
    while (successes <= 50)
    {

        {
            cv::Mat image;
            video>>image;
            
            auto x = seperatePhoto(image);
            auto matImage1 = x[0];
            auto matImage2 = x[1];
            cv::Mat matImage;
            cvtColor(matImage2, matImage, COLOR_RGB2GRAY);
            img_size.width = matImage.cols;
            img_size.height = matImage.rows;
            imshow("a", matImage);
            waitKey(0);
            found = findChessboardCorners(matImage, board_size, point_pix_pos_buf);
            // cout<<found<<endl;
            if (found && point_pix_pos_buf.size() == board_n)
            {
                successes++;
                // cout<<successes<<endl;
                find4QuadCornerSubpix(matImage, point_pix_pos_buf, Size(5, 5));
                point_pix_pos.push_back(point_pix_pos_buf);
                drawn_img = matImage.clone();
                drawChessboardCorners(matImage, board_size, point_pix_pos_buf, found);
                imshow("corners", matImage);
                waitKey(1);
            }
            else{
                // std::cout << "failed in" << std::endl;
                // imshow("corners", matImage);
                // waitKey(10);
                }
            //std::cout << "1" << std::endl;
            point_pix_pos_buf.clear();
            //std::cout << "2" << std::endl;
            //在成功调用CameraGetImageBuffer后，必须调用CameraReleaseImageBuffer来释放获得的buffer。
            //否则再次调用CameraGetImageBuffer时，程序将被挂起一直阻塞，直到其他线程中调用CameraReleaseImageBuffer来释放了buffer
        }
    }
    Size square_size(1, 1);
    std::vector<std::vector<Point3f>> point_grid_pos;
    std::vector<Point3f> point_grid_pos_buf;
    std::vector<int> point_count;
    Mat camera_matrix(3, 3, CV_32FC1, Scalar::all(0));
    Mat dist_coeffs(1, 5, CV_32FC1, Scalar::all(0));
    std::vector<Mat> rvecs;
    std::vector<Mat> tvecs;
    for (int i = 0; i < successes; i++)
    {
        for (int j = 0; j < board_h; j++)
        {
            for (int k = 0; k < board_w; k++)
            {
                Point3f pt;
                pt.x = k * square_size.width;
                pt.y = j * square_size.height;
                pt.z = 0;
                point_grid_pos_buf.push_back(pt);
            }
        }
        point_grid_pos.push_back(point_grid_pos_buf);
        point_grid_pos_buf.clear();
        point_count.push_back(board_h * board_w);
    }
    std::cout << 100 << std::endl;
    
    double temp = calibrateCamera(point_grid_pos, point_pix_pos, img_size, camera_matrix, dist_coeffs, rvecs, tvecs);
    std::cout << temp << std::endl;
    cv::FileStorage file_settings("right.yml", cv::FileStorage::WRITE);
    std::cout << camera_matrix << std::endl
              << dist_coeffs << std::endl;
    file_settings<<"camera_matrix" <<camera_matrix;
    file_settings<< "dist_coeffs"<<dist_coeffs;
    file_settings.release();
    return 0;
}



/*
左
[2164.532495233347, 0, 636.4512706056147;
 0, 3812.779355652302, 387.1529448290057;
 0, 0, 1]
[0.071274264286741, 107.610371668564, 0.003307881662813802, 0.01271162793812094, 0.8495221798740373]

右


*/


/*
you

[878.72313308412, 0, 652.3548896860685;
 0, 831.5177942954256, 373.7528333479796;
 0, 0, 1]
[0.2504010010276621, 0.2395121815623784, -0.01191231717468861, 0.01037120921368068, -0.9875812308176741]

second

[923.6059436631444, 0, 619.9664371903424;
 0, 921.9147498553101, 411.1488993580995;
 0, 0, 1]
[0.2316886352110009, 0.3166386346834036, 0.005227495968425342, 0.003530732020937117, -1.83842045350853]

zuo


[961.400713941512, 0, 603.0692661947741;
 0, 891.3845581779657, 396.0214362542773;
 0, 0, 1]
[0.1280390477756253, 2.107511609815078, 0.001047395646109399, -0.01247425027491435, -6.596049215309413]

second
[762.2457179428106, 0, 598.3124965034409;
 0, 760.4689721609482, 389.3908501730829;
 0, 0, 1]
[0.1015670190386034, 0.1614345656161827, -0.003745231470147914, -0.0002159008986214541, -0.5845529813379872]
*/


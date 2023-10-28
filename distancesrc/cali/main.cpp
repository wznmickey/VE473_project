#include <iostream>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <opencv2/highgui.hpp>
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




    const int board_w = 11,
              board_h = 11;
    const int board_n = board_w * board_h;
    Size board_size(11, 11);
    Mat gray_img, drawn_img;
    std::vector<Point2f> point_pix_pos_buf;
    std::vector<std::vector<Point2f>> point_pix_pos;
    int found, successes = 0;
    Size img_size;
    int k = 0, n = 0;
    auto video = VideoCapture(0);
    if (video.set(CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M', 'J', 'P', 'G')))
    {
        cout<<"may success in MJPG"<<endl;
    }
    if (video.set(CAP_PROP_BUFFERSIZE,1))
    {
        cout<<"may success in CAP_PROP_BUFFERSIZE"<<endl;
    }
    video.set(CAP_PROP_FRAME_WIDTH,720);
    video.set(CAP_PROP_FRAME_HEIGHT, 640);
    if (video.isOpened())
 {
     cout << "视频中图像的宽度=" << video.get(CAP_PROP_FRAME_WIDTH) << endl;
     cout << "视频中图像的高度=" << video.get(CAP_PROP_FRAME_HEIGHT) << endl;
     cout << "视频帧率=" << video.get(CAP_PROP_FPS) << endl;
     cout << "视频的总帧数=" << video.get(CAP_PROP_FRAME_COUNT);
 }
    while (successes <= 20)
    {

        {
            cv::Mat image;
            video>>image;
            imshow("a", image);
            auto x = seperatePhoto(image);
            auto matImage = x[0];
            auto matImage2 = x[1];
            waitKey(10);
            img_size.width = matImage.cols;
            img_size.height = matImage.rows;
            found = findChessboardCorners(matImage, board_size, point_pix_pos_buf);
            if (found && point_pix_pos_buf.size() == board_n)
            {
                successes++;
                find4QuadCornerSubpix(matImage, point_pix_pos_buf, Size(5, 5));
                point_pix_pos.push_back(point_pix_pos_buf);
                drawn_img = matImage.clone();
                drawChessboardCorners(matImage, board_size, point_pix_pos_buf, found);
                imshow("corners", matImage);
                waitKey(10);
            }
            else{
                std::cout << "failed in" << std::endl;
                imshow("corners", matImage);
                waitKey(10);
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
    std::cout << camera_matrix << std::endl
              << dist_coeffs << std::endl;
    return 0;
}

#include <iostream>
#include <Eigen/Dense>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/core/eigen.hpp>
#include <vector>

using namespace cv;
using namespace std;
using namespace Eigen;

vector<Mat> seperatePhoto(string st)
{
    cv::Mat image = cv::imread(st,cv::IMREAD_GRAYSCALE);

    if (image.empty()) {
        std::cerr << "无法读取图像文件" << std::endl;
        return vector<Mat>();
    }
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

void calculateDistance(vector<Mat> vec)
{

    cv::Mat disparityMap;

    cv::namedWindow("Left Half", cv::WINDOW_NORMAL);
    cv::imshow("Left Half", vec[0]);

    cv::namedWindow("Right Half", cv::WINDOW_NORMAL);
    cv::imshow("Right Half", vec[1]);

    cv::waitKey(0);



cv::Ptr<cv::StereoBM> stereo = cv::StereoBM::create();

// stereo->setBlockSize(21);        // 设置匹配块大小，尺寸必须是奇数，一般选择范围在5-21之间
// stereo->setMinDisparity(0);      // 设置最小视差值，默认为0
// stereo->setNumDisparities(16);   // 设置视差搜索范围，默认为16，即最大视差值为16
// stereo->setPreFilterSize(5);     // 设置预处理滤波器的尺寸，默认为5
// stereo->setPreFilterCap(31);     // 设置预处理滤波器的最大灰度差值，默认为31
// stereo->setTextureThreshold(10); // 设置纹理阈值，默认为10，用于筛选边缘区域
// stereo->setUniquenessRatio(15);  // 设置唯一性比率，默认为15，用于筛选匹配的唯一性

stereo->compute(vec[0], vec[1], disparityMap);
cv::Mat pointCloud;

cv::Mat Q = cv::Mat::eye(4, 4, CV_64F); // 创建一个单位矩阵作为Q矩阵

// 估计焦距（以像素为单位）
double fx = 1000.0; // 估计的焦距X
double fy = 1000.0; // 估计的焦距Y

// 估计光心坐标（以像素为单位）
double cx = vec[0].cols / 2.0; // 估计的光心X
double cy = vec[0].rows / 2.0; // 估计的光心Y

Q.at<double>(0, 3) = -cx;
Q.at<double>(1, 3) = -cy;
Q.at<double>(2, 3) = fx; // 这里将估计的焦距值赋给Q矩阵
Q.at<double>(3, 2) = -1.0 / 1; // baseline是左右摄像头之间的距离

// 在这里，baseline是左右摄像头的距离，需要根据实际情况提供

cv::reprojectImageTo3D(disparityMap, pointCloud, Q);  // Q 是标定后的校正矩阵
disparityMap.convertTo(disparityMap, CV_32F, 1.0 / 16.0f);
cv::imshow("Disparity Map", disparityMap);
// cout<<disparityMap;
cv::imshow("Point Cloud", pointCloud);
cv::waitKey(0);
return ;
}

int main()
{
    auto photo = seperatePhoto("WIN_20231018_17_05_17_Pro.jpg");
    calculateDistance(photo);
    return 0;
}




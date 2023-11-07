#include <iostream>
// #include <Eigen/Dense>
#include <opencv2/core/core.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d.hpp>
// #include <opencv2/core/eigen.hpp>
#include <vector>
#define DEBUG
using namespace cv;
using namespace std;
// using namespace Eigen;

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

    #ifdef DEBUG
    cv::namedWindow("Left Half", cv::WINDOW_NORMAL);
    cv::imshow("Left Half", vec[0]);

    cv::namedWindow("Right Half", cv::WINDOW_NORMAL);
    cv::imshow("Right Half", vec[1]);

    cv::waitKey(0);
    #endif
    
    Ptr<ORB> orb = ORB::create();
    Mat gray1,gray2;
    gray1 = vec[0].clone();
    gray2 = vec[1].clone();
    // cvtColor(vec[0],gray1,COLOR_RGB2GRAY);
    // cvtColor(vec[1],gray2,COLOR_RGB2GRAY);
    printf("123");
    vector<KeyPoint> feature_points1,feature_points2;
    // do Orient_FAST detect Keypoint
    orb->detect(gray1,feature_points1);
    orb->detect(gray2,feature_points2);
    printf("345");
    Mat descriptor1,descriptor2;
    orb->compute(gray1,feature_points1,descriptor1);
    orb->compute(gray2,feature_points2,descriptor2);
printf("456");
    BFMatcher matcher(NORM_HAMMING); //O(N^2)
    vector<DMatch> pairs;
    matcher.match(descriptor1,descriptor2,pairs);
    printf("DMatch contains the matched points like (%d in img1,%d in img2) their distance is %.3f (in Hamming Norm).\n"
           ,pairs[0].queryIdx,pairs[0].trainIdx,pairs[0].distance);
 Mat canvas;
    drawMatches(vec[0],feature_points1,vec[1],feature_points2,pairs,canvas);
        #ifdef DEBUG

    imshow("show",canvas);
    waitKey(0);
#endif

     //You can also filter the match to generate
    vector<DMatch> good;
    double min_dist = 100000;
    // compute the minimum of the distance
    for(const DMatch&m:pairs)
    {
        if(m.distance < min_dist) min_dist = m.distance;
    }
    // filter
    for(const DMatch&m:pairs)
    {
        if(m.distance < max(min_dist*2,30.))
        {
            good.push_back(m);
        }
    }
    drawMatches(vec[0],feature_points1,vec[1],feature_points2,good,canvas);
    imwrite("../good_match.jpg",canvas);
        #ifdef DEBUG

    imshow("show",canvas);
    waitKey(0);
    #endif
    drawKeypoints(vec[0],feature_points1,canvas,Scalar::all(-1),DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
    imwrite("../keypoints.jpg",canvas);
        #ifdef DEBUG

    imshow("show",canvas);
    waitKey(0);
#endif
    cv::Mat ans = vec[0].clone();
    ans.setTo(0);
    ans.convertTo(ans, CV_8UC1);
    ans = Mat(Size(vec[0].size()),CV_8UC1);
    cout<<ans.size()<<endl;
    // cout<<ans;
    cout<<ans.at<uchar>(Point(1,1))<<endl;;
    for (auto i=0;i<good.size();i++)
    {
        Point x;
        x.x = feature_points1[good[i].queryIdx].pt.x;
        x.y = feature_points1[good[i].queryIdx].pt.y;
        ans.at<uchar>(x) = good[i].distance;
        // cout<<"a" <<good[i].distance<<endl;
        // cout<<"c"<<x<<endl;
        // cout<<"b" <<ans.at<uchar>(x.x,x.y)<<endl;
    }
    // cout<<ans;
        #ifdef DEBUG

    cv::imshow("ans", ans);
    waitKey(0);
#endif
    // for (auto i :good)
    // {
    //     cout<<i.distance<<" "<<endl;
       
    // }

    // for (auto i:feature_points1)
    // {
    //     cout<<i.pt<<endl;
    // }
    // cout<<feature_points1.size()<<endl;



    // return 0;


// cv::Ptr<cv::StereoBM> stereo = cv::StereoBM::create();

// stereo->setBlockSize(9);        // 设置匹配块大小，尺寸必须是奇数，一般选择范围在5-21之间
// stereo->setMinDisparity(0);      // 设置最小视差值，默认为0
// stereo->setNumDisparities(16);   // 设置视差搜索范围，默认为16，即最大视差值为16
// stereo->setPreFilterSize(5);     // 设置预处理滤波器的尺寸，默认为5
// stereo->setPreFilterCap(31);     // 设置预处理滤波器的最大灰度差值，默认为31
// stereo->setTextureThreshold(10); // 设置纹理阈值，默认为10，用于筛选边缘区域
// stereo->setUniquenessRatio(15);  // 设置唯一性比率，默认为15，用于筛选匹配的唯一性
// // stereo->setSpeckleWindowSize(100);

// stereo->compute(vec[0], vec[1], disparityMap);

// // disparityMap.convertTo(disparityMap, CV_8U);

// // cout<<disparityMap<<endl;

// // cv::normalize(disparityMap, disparityMap,0,255,NORM_MINMAX,CV_8U);

// // cv::imshow("a", disparityMap);

// // cv::waitKey(0);

// Mat ans = disparityMap.clone();
// int type = disparityMap.type();
// ans.convertTo(ans, CV_16UC1);


// // for (auto i:ans.begin<>()){

// // }

// cout<<type<<endl;
//     float fx = 21;
//     float baseline = 100; //基线距离650mm

//     if (type == CV_8U)
//     {
//         const float PI = 3.14159265358;
//         int height = disparityMap.rows;
//         int width = disparityMap.cols;

//         uchar* dispData = (uchar*)disparityMap.data;
//         ushort* depthData = (ushort*)ans.data;
//         for (int i = 0; i < height; i++)
//         {
//             for (int j = 0; j < width; j++)
//             {
//                 int id = i*width + j;
//                 // cout<< i<<" " <<j<<" " <<height <<" " <<width<<endl;
//                 if (!dispData[id])  continue;  //防止0除
//                 depthData[id] = ushort( (float)fx *baseline / ((float)dispData[id]) );
//                 // cout<<depthData[id]<<endl;
//             }
//         }
//     }
//     // else
//     // {
//     //     cout << "please confirm dispImg's type!" << endl;
//     //     cv::waitKey(0);
//     // }

// cv::normalize(ans, ans,0,255,NORM_MINMAX,CV_8U);
// cv::imshow("a", ans);

// cv::waitKey(0);


// cv::Mat pointCloud;

// cv::Mat Q = cv::Mat::eye(4, 4, CV_64F); // 创建一个单位矩阵作为Q矩阵

// // 估计焦距（以像素为单位）
// double fx = 2000.0; // 估计的焦距X
// double fy = 2000.0; // 估计的焦距Y

// // 估计光心坐标（以像素为单位）
// double cx = 600; // 估计的光心X
// double cy = 300; // 估计的光心Y

// Q.at<double>(0, 3) = -cx;
// Q.at<double>(1, 3) = -cy;
// Q.at<double>(2, 3) = fx; // 这里将估计的焦距值赋给Q矩阵
// Q.at<double>(3, 2) = -1.0 / 1; // baseline是左右摄像头之间的距离

// // 在这里，baseline是左右摄像头的距离，需要根据实际情况提供

// cv::reprojectImageTo3D(disparityMap, pointCloud, Q);  // Q 是标定后的校正矩阵
// cv::imshow("Disparity Map", disparityMap);

// // disparityMap.convertTo(disparityMap, CV_32F, 1.0 / 16.0f);
// // cout<<disparityMap;
// // cv::imshow("Point Cloud", pointCloud);
// cv::waitKey(0);
return ;
}

int main()
{
    auto photo = seperatePhoto("WIN_20231018_17_12_43_Pro.jpg");
    calculateDistance(photo);
    return 0;
}




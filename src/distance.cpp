#include "distance.h"
#include "config.h"

static int min2(int a, int b){
    return a<b?a:b;
}

static int max2(int a, int b) {
    return a>b?a:b;
}
// Mat cameraMatrixL = (Mat_<double>(3, 3) << 475.8691, -2.9390,275.9443, 0, 476.0757, 228.3968, 0, 0, 1.);
// Mat distCoeffL = (Mat_<double>(5, 1) << -0.0760, -0.0203, 0.00061261 ,-0.0072 , 0);
// Mat cameraMatrixR = (Mat_<double>(3, 3) << 475.7032,-3.4287,257.9140, 0, 476.2003,226.5524, 0, 0, 1);
// Mat distCoeffR = (Mat_<double>(5, 1) << 0.0752,-0.0038,0.0021,-0.0076, 0);
// Mat T = (Mat_<double>(3, 1) << -24.5693,0.2246,-0.2503);//T平移向量
// Mat rec = (Mat_<double>(3, 3) << 1, 0.00085287, -0.0021,
// 	                                -0.00084804, 1, 0.0023,
// 	                            0.0021, -.0023, 1);                //rec旋转向量，对应matlab om参数  我 
// Mat R;
// Mat Q;
Mat cameraMatrixL = (Mat_<double>(3, 3) << 487.2015, 0.3897,287.4948, 0, 488.1516, 260.5195, 0, 0, 1.);
Mat distCoeffL = (Mat_<double>(5, 1) <<0.1165,-0.1455,-0.0011, 0.00073831 , 0);
Mat cameraMatrixR = (Mat_<double>(3, 3) <<486.8839, 0.5513,305.1667, 0, 487.6473,263.9588, 0, 0, 1);
Mat distCoeffR = (Mat_<double>(5, 1) << 0.1081,-0.1127, 0.00000013425 , 0.00097226 , 0);
Mat T = (Mat_<double>(3, 1) << -58.9491,0.1002,-0.1457);//T平移向量
Mat rec = (Mat_<double>(3, 3) << 1,0.0013,-0.0017,-0.0013,1,-0.0033,0.0017,0.0033,1);                //rec旋转向量，对应matlab om参数  我 
Mat R;
Mat Q;

void DistanceCalc::distcalcinit(void)
{
    //Init distance calc
    cv::FileStorage left_setting("left.yml", cv::FileStorage::READ);
    cv::FileStorage right_setting("right.yml", cv::FileStorage::READ);
    left_setting["camera_matrix"] >> left_camera_matrix;
    left_setting["dist_coeffs"] >> left_dist_coeffs;
    right_setting["camera_matrix"] >> right_camera_matrix;
    right_setting["dist_coeffs"] >> right_dist_coeffs;

    left_setting.release();
    right_setting.release();
}

DistanceCalc::DistanceCalc() {
    // this->stereo = cv::StereoBM::create();
    // this->stereo->setBlockSize(5);        // 设置匹配块大小，尺寸必须是奇数，一般选择范围在5-21之间
    // this->stereo->setMinDisparity(0);      // 设置最小视差值，默认为0
    // this->stereo->setNumDisparities(16);   // 设置视差搜索范围，默认为16，即最大视差值为16
    // this->stereo->setPreFilterSize(5);     // 设置预处理滤波器的尺寸，默认为5
    // this->stereo->setPreFilterCap(31);     // 设置预处理滤波器的最大灰度差值，默认为31
    // this->stereo->setTextureThreshold(10); // 设置纹理阈值，默认为10，用于筛选边缘区域
    // this->stereo->setUniquenessRatio(15);  // 设置唯一性比率，默认为15，用于筛选匹配的唯一性

    int              setNumDisparities    = 64;
    int              setPreFilterCap      = 1;
    int              setUniquenessRatio   = 10;
    int              setsgbmWinSize       = 8;
    // int              setP1                = 10;
    // int              setP2                = 50;
    int              setSpeckleWindowSize = 100;
    int              setSpeckleRange      = 100;
    int              setDisp12MaxDiff     = -1;
    this->sgbm = cv::StereoSGBM::create( 1, setNumDisparities );
    sgbm->setPreFilterCap( setPreFilterCap );
    // int SADWindowSize = 9;
    // int sgbmWinSize   = SADWindowSize > 0 ? SADWindowSize : 3;
    sgbm->setBlockSize( setsgbmWinSize );
    // int cn =3;
    sgbm->setP1( setsgbmWinSize * setsgbmWinSize * 8 * 3 );
    sgbm->setP2( setsgbmWinSize * setsgbmWinSize * 32 * 3 );
    sgbm->setUniquenessRatio( setUniquenessRatio );
    sgbm->setSpeckleWindowSize( setSpeckleWindowSize );
    sgbm->setSpeckleRange( setSpeckleRange );
    sgbm->setDisp12MaxDiff( setDisp12MaxDiff );

    this->distcalcinit();
}

vector<Mat> DistanceCalc::seperatePhoto(cv::Mat image)
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
    // cv::cvtColor(left_half, left_half,COLOR_BGR2GRAY);
    // cv::cvtColor(right_half, right_half,COLOR_BGR2GRAY);
    ans.push_back(left_half.clone());
    ans.push_back(right_half.clone());

    return ans;
}

vector<Mat> photoprogress(vector<Mat> vec) {

Rect validROIL;//图像校正之后，会对图像进行裁剪，这里的validROI就是指裁剪之后的区域  
Rect validROIR;
Size imageSize = Size(640, 480);
Rodrigues(rec, R); //Rodrigues变换

Mat mapLx, mapLy, mapRx, mapRy;     //映射表  
Mat Rl, Rr, Pl, Pr;              //校正旋转矩阵R，投影矩阵P 重投影矩阵Q

	stereoRectify(cameraMatrixL, distCoeffL, cameraMatrixR, distCoeffR, imageSize, R, T, Rl, Rr, Pl, Pr, Q, CALIB_ZERO_DISPARITY,
		0, imageSize, &validROIL, &validROIR);
	initUndistortRectifyMap(cameraMatrixL, distCoeffL, Rl, Pr, imageSize, CV_32FC1, mapLx, mapLy);
	initUndistortRectifyMap(cameraMatrixR, distCoeffR, Rr, Pr, imageSize, CV_32FC1, mapRx, mapRy);
Mat rectifyImageL, rectifyImageR;
cvtColor(vec[0], vec[0], COLOR_BGR2GRAY);
cvtColor(vec[1], vec[1], COLOR_BGR2GRAY);
remap(vec[0], rectifyImageL, mapLx, mapLy, INTER_LINEAR);
	remap(vec[1], rectifyImageR, mapRx, mapRy, INTER_LINEAR);

    vector<Mat> ans;
    ans.push_back(rectifyImageL);
    ans.push_back(rectifyImageR);
    return ans;
}

void DistanceCalc::calculateMap(vector<Mat> vec) {
    sgbm->setMode( cv::StereoSGBM::MODE_HH );
    // cout << vec [ 0 ].type()<<endl;
    // vec [ 0 ].convertTo( vec [ 0 ], CV_8UC1 );

    // vec [ 1 ].convertTo( vec [ 1 ], CV_8UC1 );

    vec = photoprogress(vec);
    
    sgbm->compute( vec [ 0 ], vec [ 1 ], disparityMap );
Mat disp, disp8;
// Mat xyz;  
    disp8 = Mat(disparityMap.rows, disparityMap.cols, CV_8UC1);
	normalize(disparityMap, disp8, 0, 255, NORM_MINMAX, CV_8UC1);
	reprojectImageTo3D(disparityMap, xyz, Q, true); 
	xyz = xyz * 16;
	imwrite("disparity.jpg", disp8);

    // ans = disparityMap.clone();
    // Mat show = ans.clone();
    // cv::normalize(show, show,0,255,NORM_MINMAX,CV_8U);
    // cv::imwrite("wzntmep.jpg",show);    

    int type = disparityMap.type();
    // std::cout << "type = " << type << ", CV_8U = " << CV_8U << std::endl;
    // ans.convertTo(ans, CV_16UC1);
    // cout<<disparityMap<<endl;
    this->dispData = (short int*)disparityMap.data;
    // this->depthData = (ushort*)ans.data;
}

double DistanceCalc::getBlockPropotion(cv::Rect2d roi) {
    using namespace cfg;
    struct Config conf;

    // default : 640 * 480
    int width = conf.width/2;
    int height = conf.height;

    // calculate the block size
    double area = roi.height;
    double propotion = area / float(height);
    return propotion;
}

double DistanceCalc::calculateDistance(vector<Mat> vec,cv::Rect2d roi)
{
    // auto temp = vec[0].clone();
    // cv::rectangle(temp, roi, cv::Scalar(0.2), 5);
    // cv::imwrite("./tmp.jpg", temp);

    double prop = getBlockPropotion(roi);
    if (prop >= 0.6) return (13.64*prop*prop-24.767*prop+11.474);
    if (prop < 0.1) return 20;

    float fx = 21;
    float baseline = 60; // distance between 2 cam: 60 mm
    double finalDistance = 0;
    double countedPoint = 0;

    const float PI = 3.14159265358;
    int height = disparityMap.rows;
    int width = disparityMap.cols;
    int range_h = min2(height, roi.y + roi.height);
    int range_w = min2(width, roi.x + roi.width);

    for (int i = max2(0,roi.y); i < range_h; i++)
    {
        for (int j = max2(0,roi.x); j < range_w; j++)
        {
            // int id = i*width + j;
            // cout<< i<<" " <<j<<" " <<height <<" " <<width<<endl;
            // if (!dispData[id])  continue;  //防止0除
            // if (dispData[id]<=0) continue;
            // depthData[id] = ushort( (float)fx * (float) baseline / ((float) dispData[id]) );
            auto point3 = xyz.at<Vec3f>(Point(j,i))[2];
            if (point3>100000) continue;
            if (point3<=10) continue;
            finalDistance+=point3;
            countedPoint++;
            cout<<point3<<endl;
            // cout<<depthData[id]<<endl;
        }
    }
    // }
    if (countedPoint ==0) return 100000;
    cout<<"Point coundted in the rectangle: " <<countedPoint<<endl;
    return finalDistance/countedPoint; //
}

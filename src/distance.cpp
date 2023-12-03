#include "distance.h"
#include "config.h"
#include "Detection.h"
static int min2(int a, int b){
    return a<b?a:b;
}

static int max2(int a, int b) {
    return a>b?a:b;
}

static int rectify(int left, int right) {
    int diff = right - left;
    if (diff >= 300) return (diff / 5)<<1;
    if (diff >= 100) return (diff / 3);
    return 0;
}

// Wrong, do not use
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

// camera 1
// Mat cameraMatrixL = (Mat_<double>(3, 3) << 482.5015,0.1909,298.9531, 0, 483.5012,211.7897, 0, 0, 1.);
// Mat distCoeffL = (Mat_<double>(5, 1) << 0.1072,-0.1186,-0.0013,0.0027,0);
// Mat cameraMatrixR = (Mat_<double>(3, 3) << 481.6718,-0.0082,276.8195, 0, 482.3926,209.6866, 0, 0, 1);
// Mat distCoeffR = (Mat_<double>(5, 1) << 0.0984,-0.0805,0.00085982,0.0023, 0);
// Mat T = (Mat_<double>(3, 1) << -56.4343,-0.2152,-0.6567);//T平移向量
// Mat rec = (Mat_<double>(3, 3) << 1,0.00069102,0.00022640,-0.00069164,1,0.0027,-0.00022452,-0.0027,1);                //rec旋转向量，对应matlab om参数  我 
// Mat R;
// Mat Q;

// camera 2
// Mat cameraMatrixL = (Mat_<double>(3, 3) << 487.2015, 0.3897,287.4948, 0, 488.1516, 260.5195, 0, 0, 1.);
// Mat distCoeffL = (Mat_<double>(5, 1) <<0.1165,-0.1455,-0.0011, 0.00073831 , 0);
// Mat cameraMatrixR = (Mat_<double>(3, 3) <<486.8839, 0.5513,305.1667, 0, 487.6473,263.9588, 0, 0, 1);
// Mat distCoeffR = (Mat_<double>(5, 1) << 0.1081,-0.1127, 0.00000013425 , 0.00097226 , 0);
// Mat T = (Mat_<double>(3, 1) << -58.9491,0.1002,-0.1457);//T平移向量
// Mat rec = (Mat_<double>(3, 3) << 1,0.0013,-0.0017,-0.0013,1,-0.0033,0.0017,0.0033,1);                //rec旋转向量，对应matlab om参数  我 
// Mat R;
// Mat Q;

// void DistanceCalc::distcalcinit(void)
// {
//     //Init distance calc
//     cv::FileStorage left_setting("left.yml", cv::FileStorage::READ);
//     cv::FileStorage right_setting("right.yml", cv::FileStorage::READ);
//     left_setting["camera_matrix"] >> left_camera_matrix;
//     left_setting["dist_coeffs"] >> left_dist_coeffs;
//     right_setting["camera_matrix"] >> right_camera_matrix;
//     right_setting["dist_coeffs"] >> right_dist_coeffs;

//     left_setting.release();
//     right_setting.release();
// }

DistanceCalc::DistanceCalc() {
    int              setNumDisparities    = 16;
    int              setPreFilterCap      = 1;
    int              setUniquenessRatio   = 5;
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

    // Rect validROIL;// After calibration, the figure need to be cut
    // Rect validROIR;
    // Size imageSize = Size(640, 480);
    // Rodrigues(rec, R); //Rodrigues Transformation


    // Mat Rl, Rr, Pl, Pr;
    // stereoRectify(cameraMatrixL, distCoeffL, cameraMatrixR, distCoeffR, imageSize, R, T, Rl, Rr, Pl, Pr, Q, CALIB_ZERO_DISPARITY,
    //               0, imageSize, &validROIL, &validROIR);

                  
    // initUndistortRectifyMap(cameraMatrixL, distCoeffL, Rl, Pr, imageSize, CV_32FC1, mapLx, mapLy);
    // initUndistortRectifyMap(cameraMatrixR, distCoeffR, Rr, Pr, imageSize, CV_32FC1, mapRx, mapRy);
 
    //this->distcalcinit();
}

// vector<Mat> DistanceCalc::photoprogress(vector<Mat> & vec) {
//     struct timeval startTime;
//     gettimeofday(&startTime,NULL);
    
//     Mat rectifyImageL, rectifyImageR;
//     cvtColor(vec[0], vec[0], COLOR_BGR2GRAY);
//     cvtColor(vec[1], vec[1], COLOR_BGR2GRAY);
//     remap(vec[0], rectifyImageL, mapLx, mapLy, INTER_LINEAR);
//     remap(vec[1], rectifyImageR, mapRx, mapRy, INTER_LINEAR);

//     vector<Mat> ans;
//     ans.push_back(rectifyImageL);
//     ans.push_back(rectifyImageR);
//     struct timeval endTime;
//     gettimeofday(&endTime,NULL);
//     cout << "Photo calibrate time: "<<timeDiff(startTime,endTime)<<endl;
//     return ans;
// }

void DistanceCalc::calculateMap(vector<Mat> & vec, cv::Mat & Q) {
         
    sgbm->setMode( cv::StereoSGBM::MODE_HH );
    // sgbm->setMode( cv::StereoSGBM::MODE_SGBM_3WAY );
    // cout << vec [ 0 ].type()<<endl;
    // vec [ 0 ].convertTo( vec [ 0 ], CV_8UC1 );

    // vec [ 1 ].convertTo( vec [ 1 ], CV_8UC1 );

    // vec = photoprogress(vec);
    struct timeval startTime;
    gettimeofday(&startTime,NULL);
    sgbm->compute( vec [0], vec [1], disparityMap );
    Mat disp, disp8;  
    disp = disparityMap.clone();
    disp.convertTo( disp, CV_8UC1,0.25);
    // disp8 = Mat(disparityMap.rows, disparityMap.cols, CV_8UC1);
    
    // applyColorMap( disp, disp, COLORMAP_RAINBOW );


	// normalize(disparityMap, disp8, 0, 255, NORM_MINMAX, CV_8UC1);
	reprojectImageTo3D(disparityMap, xyz, Q, true); 
	xyz = xyz * 16;
	imwrite("disparity.jpg", disp);

    // ans = disparityMap.clone();
    // Mat show = ans.clone();
    // cv::normalize(show, show,0,255,NORM_MINMAX,CV_8U);
    // cv::imwrite("wzntmep.jpg",show);    

    // int type = disparityMap.type();
    // std::cout << "type = " << type << ", CV_8U = " << CV_8U << std::endl;
    // ans.convertTo(ans, CV_16UC1);
    // cout<<disparityMap<<endl;
    // this->dispData = (short int*)disparityMap.data;
    // this->depthData = (ushort*)ans.data;
    struct timeval endTime;
    gettimeofday(&endTime,NULL);
    cout << "Map calculate time: "<<timeDiff(startTime,endTime)<<endl;

}

double DistanceCalc::getBlockPropotion(cv::Rect2d & roi) {
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

double DistanceCalc::calculateDistance(vector<Mat> & vec,cv::Rect2i & roi)
{
    struct timeval startTime;
    gettimeofday(&startTime,NULL);

    // auto temp = vec[0].clone();
    // cv::rectangle(temp, roi, cv::Scalar(0.2), 5);
    // cv::imwrite("./tmp.jpg", temp);

    // double prop = getBlockPropotion(roi);
    // if (prop >= 0.6) return (13.64*prop*prop-24.767*prop+11.474);
    // if (prop < 0.1) return 20;

    double finalDistance = 0;
    double countedPoint = 0;

    int height = disparityMap.rows;
    int width = disparityMap.cols;

    int height_up = min2(height, roi.y + roi.height);
    int width_up = min2(width, roi.x + roi.width);
    int height_down = max2(0, roi.y);
    int width_down = max2(0, roi.x);

    int recti = rectify(height_down, height_up);
    height_down += recti;
    height_up -= recti;
    recti = rectify(width_down, width_up);
    width_up -= recti;
    width_down += recti;

    for (int i = height_down; i < height_up; i++)
    {
        for (int j = width_down; j < width_up; j++)
        {
            auto point3 = xyz.at<Vec3f>(Point(j,i))[2];
            if (point3 > 100000 || point3 <= 10) continue;
            finalDistance+=point3;
            countedPoint++;
        }
    }
    if (countedPoint ==0) return 100000;
    cout<<"Point coundted in the rectangle: " << countedPoint <<endl;

    struct timeval endTime;
    gettimeofday(&endTime,NULL);
    cout << "Distance calc time: "<<timeDiff(startTime,endTime)<<endl;

    return finalDistance/countedPoint/(double)2000; //
}

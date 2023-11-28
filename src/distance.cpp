#include "distance.h"
#include "config.h"

static int min2(int a, int b){
    return a<b?a:b;
}

static int max2(int a, int b) {
    return a>b?a:b;
}

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
    this->stereo = cv::StereoBM::create();
    this->stereo->setBlockSize(5);        // 设置匹配块大小，尺寸必须是奇数，一般选择范围在5-21之间
    this->stereo->setMinDisparity(0);      // 设置最小视差值，默认为0
    this->stereo->setNumDisparities(16);   // 设置视差搜索范围，默认为16，即最大视差值为16
    this->stereo->setPreFilterSize(5);     // 设置预处理滤波器的尺寸，默认为5
    this->stereo->setPreFilterCap(31);     // 设置预处理滤波器的最大灰度差值，默认为31
    this->stereo->setTextureThreshold(10); // 设置纹理阈值，默认为10，用于筛选边缘区域
    this->stereo->setUniquenessRatio(15);  // 设置唯一性比率，默认为15，用于筛选匹配的唯一性
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


void DistanceCalc::calculateMap(vector<Mat> vec) {
    int              setNumDisparities    = 3;
    int              setPreFilterCap      = 4;
    int              setUniquenessRatio   = 15;
    int              setsgbmWinSize       = 10;
    // int              setP1                = 10;
    // int              setP2                = 50;
    int              setSpeckleWindowSize = 85;
    int              setSpeckleRange      = 88;
    int              setDisp12MaxDiff     = -1;

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

    ans = disparityMap.clone();
    Mat show = ans.clone();
    cv::normalize(show, show,0,255,NORM_MINMAX,CV_8U);
    cv::imwrite("show.jpg",show);    

    int type = disparityMap.type();
    // std::cout << "type = " << type << ", CV_8U = " << CV_8U << std::endl;
    ans.convertTo(ans, CV_16UC1);
    // cout<<disparityMap<<endl;
    this->dispData = (short int*)disparityMap.data;
    this->depthData = (ushort*)ans.data;
}

double DistanceCalc::getBlockPropotion(cv::Rect2d roi) {
    using namespace cfg;
    struct Config conf;

    // default : 640 * 480
    int width = conf.width/2;
    int height = conf.height;

    // calculate the block size
    double area = roi.area();
    double propotion = area / float(width * height);
    return propotion;
}

double DistanceCalc::calculateDistance(vector<Mat> vec,cv::Rect2d roi)
{
    // auto temp = vec[0].clone();
    // cv::rectangle(temp, roi, cv::Scalar(0.2), 5);
    // cv::imwrite("./tmp.jpg", temp);

    double prop = getBlockPropotion(roi);
    if (prop >= 0.7) return (10 / prop);
    if (prop < 0.01) return 100;

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
            int id = i*width + j;
            // cout<< i<<" " <<j<<" " <<height <<" " <<width<<endl;
            // if (!dispData[id])  continue;  //防止0除
            if (dispData[id]<=0) continue;
            depthData[id] = ushort( (float)fx * (float) baseline / ((float) dispData[id]) );
            
            finalDistance+=depthData[id];
            countedPoint++;
            
            // cout<<depthData[id]<<endl;
        }
    }
    // }
    cout<<"Point coundted in the rectangle: " <<countedPoint<<endl;
    return finalDistance/countedPoint; //
}

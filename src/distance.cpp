#include "distance.h"


Mat left_camera_matrix;
Mat left_dist_coeffs;
Mat right_camera_matrix;
Mat right_dist_coeffs;

void distcalcinit(void)
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


double calculateDistance(vector<Mat> vec, cv::Rect2d roi)
{
    auto temp = vec[0].clone();
    // cv::rectangle(temp, roi, cv::Scalar(0.2), 5);
    cv::imwrite("./gray.jpg", temp);

    cv::Mat disparityMap;

    cv::Ptr<cv::StereoBM> stereo = cv::StereoBM::create();

    stereo->setBlockSize(7);        // 设置匹配块大小，尺寸必须是奇数，一般选择范围在5-21之间
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
    Mat show = ans.clone();
    cv::normalize(show, show,0,255,NORM_MINMAX,CV_8U);
    cv::imwrite("show.jpg",show);    


    int type = disparityMap.type();
    ans.convertTo(ans, CV_16UC1);

    float fx = 21;
    float baseline = 100; //基线距离650mm
    double finalDistance = 0;
    auto countedPoint = 0;
    std::cout << "type = " << type << ", CV_8U = " << CV_8U << std::endl;
    // if (type == CV_8U) {

        const float PI = 3.14159265358;
        int height = disparityMap.rows;
        int width = disparityMap.cols;

        short int* dispData = (short int*)disparityMap.data;
        ushort* depthData = (ushort*)ans.data;
    priority_queue<float,vector<float>,greater<float>>q;
	for (int i = 0; i < height; i++)
        {
            for (int j = 0; j < width; j++)
            {
                int id = i*width + j;
                // cout<< i<<" " <<j<<" " <<height <<" " <<width<<endl;
                if (!dispData[id])  continue;  //防止0除
                depthData[id] = ushort( (float)fx * (float) baseline / ((float) dispData[id]) );
                
                if (i>=roi.y && i<=roi.y+roi.height && j>=roi.x && j<=roi.x+roi.width)
                {
                    // finalDistance += depthData[id];
                    if ((float)fx * (float) baseline / ((float) dispData[id])<1) continue;
                    countedPoint++;
                    q.push((float)fx * (float) baseline / ((float) dispData[id]));
                }
                
                // cout<<depthData[id]<<endl;
            }
        }
    int mid = countedPoint/2;
    int sm = mid - countedPoint/10;
    int lm = mid + countedPoint/10;
    int finalCount = 0;
    for (int i=0;i<countedPoint;i++)
    {
        if (q.empty()){
            break;
        }
        auto temp = q.top();
        q.pop();
        if (i>sm && i<lm){
            finalDistance +=temp;
            finalCount++;
        }
    }
    cout<<"Points counted: " <<finalCount<<endl;
    
return finalDistance/finalCount; //
}


double calculateDistanceSIFT(vector<Mat> vec, cv::Rect2d roi)
{

    cv::Mat disparityMap;

    Ptr<ORB> orb = ORB::create();
    Mat gray1,gray2;
    gray1 = vec[0].clone();
    gray2 = vec[1].clone();
    
    // cvtColor(vec[0],gray1,COLOR_RGB2GRAY);
    // cvtColor(vec[1],gray2,COLOR_RGB2GRAY);
    printf("Detecting feature points...\n");
    vector<KeyPoint> feature_points1,feature_points2;
    // do Orient_FAST detect Keypoint
    orb->detect(gray1,feature_points1);
    orb->detect(gray2,feature_points2);
    printf("Computing descriptors...\n");

    Mat descriptor1,descriptor2;
    orb->compute(gray1,feature_points1,descriptor1);
    orb->compute(gray2,feature_points2,descriptor2);
    printf("Matching pairs...\n");
    BFMatcher matcher(NORM_HAMMING); //O(N^2)
    vector<DMatch> pairs;
    matcher.match(descriptor1,descriptor2,pairs);
    printf("DMatch contains the matched points like (%d in img1, %d in img2) their distance is %.3f (in Hamming Norm).\n"
           ,pairs[0].queryIdx,pairs[0].trainIdx,pairs[0].distance);
    Mat canvas;
    drawMatches(vec[0],feature_points1,vec[1],feature_points2,pairs,canvas);

    // You can also filter the match to generate
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

    drawKeypoints(vec[0],feature_points1,canvas,Scalar::all(-1),DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
    imwrite("../keypoints.jpg",canvas);

    cv::Mat ans = vec[0].clone();
    ans.setTo(0);
    ans.convertTo(ans, CV_8UC1);
    ans = Mat(Size(vec[0].size()),CV_8UC1);
    cout<<ans.size()<<endl;
    // cout<<ans;
    cout<<ans.at<uchar>(Point(1,1))<<endl;;

    int countedPoint = 0;
    double distanceSum = 0.0;
    for (auto i=0;i<good.size();i++)
    {
        Point x;
        x.x = feature_points1[good[i].queryIdx].pt.x;
        x.y = feature_points1[good[i].queryIdx].pt.y;

        if (x.y>=roi.y && x.y<=roi.y+roi.height && x.x>=roi.x && x.x<=roi.x+roi.width) {
            distanceSum += good[i].distance;
            countedPoint++;  
        }
        ans.at<uchar>(x) = good[i].distance;
    }
    distanceSum = distanceSum / countedPoint;
    return distanceSum;
}
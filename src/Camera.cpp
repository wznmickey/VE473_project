#include "Camera.h"
#include <iostream>
extern cfg::Config config;
Camera::Camera(int deviceid, int camid)
{
    if(camid != 1 && camid != 2)
    {
        std::cerr << "Not valid camera id: " << camid << " !" << std::endl;
        exit(-1);
    }

    this->deviceid = (int)deviceid;
    this->camid = camid;
    cap = new cv::VideoCapture(deviceid);
    if (!cap->isOpened()) {
        std::cerr << "Cannot open camera " << deviceid << std::endl;
        exit(-1);
    }
    std::cout << "Camera " << deviceid << " init succeed" << std::endl;
    cam_init();
}

Camera::Camera(std::string filename)
{
    this->filename = filename;
    cap = new cv::VideoCapture(filename);
    if (!cap->isOpened()) {
        std::cerr << "Cannot open video: "<< filename << std::endl;
        exit(-1);
    }
}

/**
 * @brief Set the camera according to configs
 * 
 */
void Camera::cam_init(void)
{
    cap->set(3, config.width);
    cap->set(4, config.height);
    cap->set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M','J','P','G'));
    //cap.set(5, config.fps);
    using cv::Mat;
    using cv::Mat_;
    using cv::Rect;
    using cv::Size;
    Mat cameraMatrixL;
    Mat distCoeffL;
    Mat cameraMatrixR;
    Mat distCoeffR;
    Mat T;
    Mat rec;
    Mat R;
    if(this->camid == 1)
    {
        cameraMatrixL = (Mat_<double>(3, 3) << 482.5015,0.1909,298.9531, 0, 483.5012,211.7897, 0, 0, 1.);
        distCoeffL = (Mat_<double>(5, 1) << 0.1072,-0.1186,-0.0013,0.0027,0);
        cameraMatrixR = (Mat_<double>(3, 3) << 481.6718,-0.0082,276.8195, 0, 482.3926,209.6866, 0, 0, 1);
        distCoeffR = (Mat_<double>(5, 1) << 0.0984,-0.0805,0.00085982,0.0023, 0);
        T = (Mat_<double>(3, 1) << -56.4343,-0.2152,-0.6567);//T平移向量
        rec = (Mat_<double>(3, 3) << 1,0.00069102,0.00022640,-0.00069164,1,0.0027,-0.00022452,-0.0027,1);                //rec旋转向量，对应matlab om参数  我     
    }
    else if (this->camid == 2)
    {
        cameraMatrixL = (Mat_<double>(3, 3) << 487.2015, 0.3897,287.4948, 0, 488.1516, 260.5195, 0, 0, 1.);
        distCoeffL = (Mat_<double>(5, 1) <<0.1165,-0.1455,-0.0011, 0.00073831 , 0);
        cameraMatrixR = (Mat_<double>(3, 3) <<486.8839, 0.5513,305.1667, 0, 487.6473,263.9588, 0, 0, 1);
        distCoeffR = (Mat_<double>(5, 1) << 0.1081,-0.1127, 0.00000013425 , 0.00097226 , 0);
        T = (Mat_<double>(3, 1) << -58.9491,0.1002,-0.1457);//T平移向量
        rec = (Mat_<double>(3, 3) << 1,0.0013,-0.0017,-0.0013,1,-0.0033,0.0017,0.0033,1);                //rec旋转向量，对应matlab om参数  我 
    }
    Rect validROIL;// After calibration, the figure need to be cut
    Rect validROIR;
    Size imageSize = Size(640, 480);
    cv::Rodrigues(rec, R); //Rodrigues Transformation

    Mat Rl, Rr, Pl, Pr;
    cv::stereoRectify(cameraMatrixL, distCoeffL, cameraMatrixR, distCoeffR, imageSize, R, T, Rl, Rr, Pl, Pr, this->Q, cv::CALIB_ZERO_DISPARITY,
                0, imageSize, &validROIL, &validROIR);
                
    cv::initUndistortRectifyMap(cameraMatrixL, distCoeffL, Rl, Pr, imageSize, CV_32FC1, this->mapLx, this->mapLy);
    cv::initUndistortRectifyMap(cameraMatrixR, distCoeffR, Rr, Pr, imageSize, CV_32FC1, this->mapRx, this->mapRy);

}

void Camera::progress_photo(void)
{
    struct timeval startTime;
    gettimeofday(&startTime,NULL);
    
    cv::Mat rectifyImageL, rectifyImageR;
    cv::cvtColor(this->frameLeft, this->frameLeft, cv::COLOR_BGR2GRAY);
    cv::cvtColor(this->frameRight, this->frameRight, cv::COLOR_BGR2GRAY);
    remap(this->frameLeft, rectifyImageL, this->mapLx, this->mapLy, cv::INTER_LINEAR);
    remap(this->frameRight, rectifyImageR, this->mapRx, this->mapRy, cv::INTER_LINEAR);

    this->frameLeft = rectifyImageL;
    this->frameRight = rectifyImageR;

    struct timeval endTime;
    gettimeofday(&endTime,NULL);
    std::cout << "Photo calibrate time: "<< timeDiff(startTime,endTime) << std::endl;

}

/**
 * @brief Takes the image and save it.
 * 
 * @param split Whether split the image into two. Default true.
 * @return cv::Mat Image as a whole
 */
cv::Mat Camera::take_pic(bool split)
{
    cap->read(frame);
    if (this->isVid())
    {
        if (frame.empty())
        {
            std::cout << "End of video" << std::endl;
            return frame; 
        }
    }
    if(split)
    {
        split_pic();
    } 
    return frame;
}

/**
 * @brief Split the stored picture into two parts and save them 
 * 
 */
void Camera::split_pic(void)
{
    if(frame.empty())
    {
        std::cerr << "Frame is empty! Can not split.\n";
        if(this->isCam()) std::cerr << "Error in " << this->deviceid << std::endl;
        else std::cerr << "Error in " << this->filename << std::endl;
        return;
    } 
    cv::Rect left_roi(0, 0, config.width/2, config.height);
    frameLeft = frame(left_roi);
    cv::Rect right_roi(config.width/2, 0, config.width/2, config.height);
    frameRight = frame(right_roi);
}

/**
 * @brief Get a image taken.
 * 
 * @param p Which image to get. Default COMPLETE.
 * @return cv::Mat 
 */
cv::Mat Camera::get_pic(WhichCam p)
{
    switch (p)
    {
    case LEFT:
        return frameLeft;
    case RIGHT:
        return frameRight;
    default:
        return frame;
    }
}

/**
 * @brief Save a image to local. Path is defined in config.h
 * 
 * @param p Which image to save.
 * @param img_name Save file name. Default "img.jpg"
 */
void Camera::save_pic(WhichCam p, std::string img_name = "img.jpg")
{
    switch (p)
    {
    case LEFT:
        if(!frameLeft.empty()) cv::imwrite(config.savepath+img_name, frameLeft);
        else std::cerr << "Frame is empty! Can not save Left.\n";
        break;
    case RIGHT:
        if(!frameRight.empty()) cv::imwrite(config.savepath+img_name, frameRight);
        else std::cerr << "Frame is empty! Can not save Right.\n";
        break;
    default:
        if(!frame.empty()) cv::imwrite(config.savepath+img_name, frame);
        else std::cerr << "Frame is empty! Can not save Complete.\n";
        break;
    }
}

Camera::~Camera()
{
    cap->release();
    if (cap != nullptr)delete cap;
}
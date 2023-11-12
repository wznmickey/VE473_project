#include "Camera.h"
#include <iostream>
extern cfg::Config config;
Camera::Camera(int camid)
{
    this->camid = (int)camid;
    cap = new cv::VideoCapture(camid);
    if (!cap->isOpened()) {
        std::cerr << "Cannot open camera";
        exit(-1);
    }
    cam_init();
}

Camera::Camera(std::string filename)
{
    this->filename = filename;
    cap = new cv::VideoCapture(filename);
    if (!cap->isOpened()) {
        std::cerr << "Cannot open video: "<<filename<<std::endl;
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
    //cap.set(5, config.fps);
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
    if(split) split_pic();
    return frame;
}

/**
 * @brief Split the stored picture into two parts and save them 
 * 
 */
void Camera::split_pic(void)
{
    if(frame.empty()) std::cerr << "Frame is empty! Can not split.\n";
    cv::Rect left_roi(0, 0, config.width/2, config.height);
    frameLeft = frame(left_roi);
    cv::Rect right_roi(config.width/2, 0, config.width/2-1, config.height-1);
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
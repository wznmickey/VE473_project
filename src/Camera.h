#ifndef CAMERA_H
#define CAMERA_H

#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "config.h"

/**
 * @brief COMPLETE (0): Both left and right. 
 *        LEFT (1): Left half. 
 *        RIGHT (2): Right half.
 * 
 */
enum WhichCam
{
    COMPLETE,
    LEFT,
    RIGHT
};

/**
 * @brief The class for managing a single camera
 * 
 */
class Camera
{
    public:
        Camera(int camid = 0);
        Camera(std::string filename);
        ~Camera();

    private:
        //cv::VideoCapture cap = cv::VideoCapture(0);
        cv::VideoCapture * cap = nullptr;
        cv::Mat frame;
        cv::Mat frameLeft;
        cv::Mat frameRight;

        int camid = -1;
        std::string filename = "";
        void cam_init(void);
        void split_pic(void);
        bool isCam(){return camid != -1;}
        bool isVid(){return filename != "";}

    public:
        cv::Mat take_pic(bool split = true);
        cv::Mat get_pic(WhichCam p = COMPLETE);
        void save_pic(WhichCam p, std::string img_name);

};

#endif
#ifndef CAMERA_H
#define CAMERA_H

#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "config.h"
#include <sys/time.h>
#include "Detection.h"

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <opencv2/core/types.hpp>
#include <opencv2/videoio.hpp>
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <vector>
#include <opencv2/core/core.hpp>

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
        Camera(){};
        Camera(int deviceid, int camid = 1);
        Camera(std::string filename, int camid = 1);
        ~Camera();

    private:
        //cv::VideoCapture cap = cv::VideoCapture(0);
        cv::VideoCapture * cap = nullptr;
        cv::Mat frame;
        cv::Mat frameLeft;
        cv::Mat frameRight;
        int frameNum=0;
        int camid = -1;
        int deviceid = -1;
        std::string filename = "";
        void cam_init(void);
        void split_pic(void);
        bool isCam(){return camid != -1;}
        bool isVid(){return filename != "";}
    public:
        cv::Mat take_pic(bool split = true);
        cv::Mat get_pic(WhichCam p = COMPLETE);
        void save_pic(WhichCam p, std::string img_name);
        void progress_photo(void);
        cv::Mat & getQ(void){return this->Q;}
        int getcamid(void){return this->camid;}
        void resetVid(void);
        bool isEmpty(void){return this->frame.empty();}

    //Distance Configs
    private:
        cv::Mat Q;
        cv::Mat mapLx, mapLy, mapRx, mapRy;

};

#endif
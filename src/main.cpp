#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <cstdlib>
#include <signal.h>
#include <fstream>
#include "Camera.h"
#include "config.h"
#include "car.h"
#include "car_detection.h"
#include <unistd.h>
#include "distance.h"

bool flag = true;
void signal_callback_handler(int signum) {
   flag = false;
}

cfg::Config config;

int main()
{
    signal(SIGINT, signal_callback_handler);
    Camera camera("/home/pi/473_test/hit_car_right.mp4");
    //Camera camera(0);
    Car_Detection detect;
    //Init distcalc
    distcalcinit();
	while (flag) {
        cv::Mat frame = camera.take_pic();
        if (frame.empty()) break;
        cv::Mat frameL = camera.get_pic(LEFT);
        vector<cv::Rect2d> roi_vec = detect.callNetworks(frameL);

        if(!roi_vec.empty())
        {
            for(cv::Rect2d roi: roi_vec)
            {
                double dist = calculateDistance(seperatePhoto(camera.get_pic(COMPLETE)),roi);
                std::cout << "Distance: " << dist << std::endl;
            }
        }
        //camera.save_pic(LEFT, "imageL.jpg");
        //camera.save_pic(RIGHT, "imageR.jpg");
	}
	return 0;
}
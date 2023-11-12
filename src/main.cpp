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

bool flag = true;
void signal_callback_handler(int signum) {
   flag = false;
}

cfg::Config config;

int main()
{
    signal(SIGINT, signal_callback_handler);
    Camera camera("/home/zjche/Desktop/VE473_project/video/1.avi");
    //Camera camera(0);
    Car_Detection detect;
	while (flag) {
        cv::Mat frame = camera.take_pic();
        if (frame.empty()) break;
        cv::Mat frameL = camera.get_pic(LEFT);
        detect.callNetworks(frameL);
        sleep(1);
        //camera.save_pic(LEFT, "imageL.jpg");
        //camera.save_pic(RIGHT, "imageR.jpg");
	}
	return 0;
}
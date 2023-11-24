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
#include "Detection.h"
#include "Gyro.h"
#include "buzzer.h"

bool flag = true;
int ForceExit = 0;
void signal_callback_handler(int signum) {
    ForceExit ++;
    if (ForceExit >=5) exit(0);
    flag = false;
}

cfg::Config config;

int main()
{
    signal(SIGINT, signal_callback_handler);
    Camera camera("/home/pi/473_test/hit_car_right.mp4");
    //Camera camera(0);
    //Car_Detection detect;
    //Init distcalc
    Detection detection;
    Gyro gyro;

    DistanceCalc distances;
	while (flag) {
        buzzer(1000);
        // gyro.GyroTurn();
        sleep(1);
        continue;
        struct timeval startTime;
        gettimeofday(&startTime, NULL);
        cv::Mat frame;
        for (int i = 0; i < 1 ; i++)
        {
            frame = camera.take_pic();
        }
        if (frame.empty()) break;
        cv::Mat frameL = camera.get_pic(LEFT);
        //vector<cv::Rect2d> roi_vec = detect.callNetworks(frameL);
        detection.detect( frameL );
        std::vector<cv::Mat> separated_photo = distances.seperatePhoto(camera.get_pic(COMPLETE));
        distances.calculateMap(separated_photo);
        vector<cv::Rect2i> roi_vec = detection.get( );
        struct timeval endTime;
        gettimeofday(&endTime, NULL);
        std::cout << "Time for inference a frame: " << timeDiff(startTime, endTime) << std::endl;

        if(!roi_vec.empty())
        {
            for(cv::Rect2i roi: roi_vec)
            {
                struct timeval start1;
                gettimeofday(&start1, NULL);
                double dist = distances.calculateDistance(separated_photo, roi);
                std::cout << "Distance: " << dist << std::endl;
                detection.drawRectText(roi,std::to_string((int)dist));
                struct timeval end1;
                gettimeofday(&end1, NULL);
                std::cout << "Time for processing a car: " << timeDiff(start1, end1) << std::endl;
            }
            detection.ImgSave("/home/pi/VE473_project/img/result.png");
            // char q;
            // std::cin.get(q);
            // if (q == 'a') return 0;
        }
        //sleep(2);
        //camera.save_pic(LEFT, "imageL.jpg");
        //camera.save_pic(RIGHT, "imageR.jpg");
	}
	return 0;
}

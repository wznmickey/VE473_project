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
#include "Mode.h"

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
    Camera camera0("/home/pi/videos/good_videos/hit_car_left.mp4");
    Camera camera1("/home/pi/videos/good_videos/hit_car_right.mp4");
    //Camera camera0(0);
    //Camera camera1(2);
    //Init distcalc
    Gyro gyro;
    Buzzer buzzer;
    DistanceCalc distances;
    Detection detection;
    int frameCount = 0;
    Mode mode = PERFORMANCE;

    while(flag)
    {
        #ifdef PERF_DEBUG
            mode = PERFORMANCE;
        #endif
        #ifdef EFF_DEBUG
            mode = EFFICIENCY;
        #endif
        if(mode == PERFORMANCE)
        {
            if(!PerformanceMode(&camera0, &camera1, &gyro, &buzzer, &distances, &detection, &mode)) break;
        } 
        else 
        {
            if(!EfficiencyMode(&camera0, &camera1, &gyro, &buzzer, &distances, &detection, &mode)) break;
        }
        frameCount++;
        std::cout << frameCount << std::endl;
    }

	// while (flag && frameCount <= 500) {
    //     // gyro.GyroTurn();
    //     struct timeval startTime;
    //     gettimeofday(&startTime, NULL);
    //     cv::Mat frame;
    //     for (int i = 0; i < 1 ; i++)
    //     {
    //         frame = camera0.take_pic();
    //     }
    //     if (frame.empty()) break;
    //     cv::Mat frameL = camera0.get_pic(LEFT);
    //     //vector<cv::Rect2d> roi_vec = detect.callNetworks(frameL);
    //     detection.detect( frameL );
    //     std::vector<cv::Mat> separated_photo;
    //     separated_photo.emplace_back(camera0.get_pic(LEFT));
    //     separated_photo.emplace_back(camera0.get_pic(RIGHT));
    //     distances.calculateMap(separated_photo);
    //     vector<cv::Rect2i> roi_vec = detection.get( );
    //     struct timeval endTime;
    //     gettimeofday(&endTime, NULL);
    //     std::cout << "Time for inference a frame: " << timeDiff(startTime, endTime) << std::endl;

    //     if(!roi_vec.empty())
    //     {
    //         for(cv::Rect2i roi: roi_vec)
    //         {
    //             struct timeval start1;
    //             gettimeofday(&start1, NULL);
    //             double dist = distances.calculateDistance(separated_photo, roi);
    //             std::cout << "Distance: " << dist << std::endl;
    //             if (dist < 1.0) buzzer.buzz(500);

    //             detection.drawRectText(roi,std::to_string(dist).substr(0, 4));
    //             struct timeval end1;
    //             gettimeofday(&end1, NULL);
    //             // std::cout << "Time for processing a car: " << timeDiff(start1, end1) << std::endl;
    //         }
    //         frameCount += 1;
    //         string filename = "../img/result" + std::to_string(frameCount) + ".jpg";
    //         detection.ImgSave(filename);
    //         // char q;
    //         // std::cin.get(q);
    //         // if (q == 'a') return 0;
    //     }
    //     usleep(500000);
    //     //camera.save_pic(LEFT, "imageL.jpg");
    //     //camera.save_pic(RIGHT, "imageR.jpg");
	// }
	return 0;
}

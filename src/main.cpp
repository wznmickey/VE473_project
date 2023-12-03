#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <sstream>
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

int main(int argc, char* argv[])
{
    Camera * camera0;
    Camera * camera1;
    if (argc == 1)
    {
        camera0 = new Camera(0);
        camera1 = new Camera(2);
    }
    else if (argc == 3)
    {
        if (!strcmp(argv[1],"2") && !strcmp(argv[2],"0"))
        {

        }
        else if (!strcmp(argv[1],"0") && !strcmp(argv[2],"2"))
        {

        }
        else
        {
            unsigned int result;
            std::istringstream iss(argv[1]); 
            if (iss >> result) camera0 = new Camera(result);
            else camera0 = new Camera(std::string(argv[1]));
            iss = std::istringstream(argv[2]); 
            if (iss >> result) camera1 = new Camera(result);
            else camera1 = new Camera(std::string(argv[2]));
        }
    }
    else
    {
        std::cerr << "Wrong number of arguments! Aborting" << std::endl;
        return 0;
    }

    signal(SIGINT, signal_callback_handler);
    // Camera camera0("/home/pi/videos/good_videos/hit_car_left.mp4");
    // Camera camera1("/home/pi/videos/good_videos/hit_car_right.mp4");
    // Camera camera0("/home/pi/2_cam.mp4");
    // Camera camera0("/home/pi/zjc1.mp4");
    // Camera camera1(2);
    // Init distcalc
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
            if(!PerformanceMode(*camera0, *camera1, gyro, buzzer, distances, detection, mode)) break;
        } 
        else 
        {
            if(!EfficiencyMode(*camera0, *camera1, gyro, buzzer, distances, detection, mode)) break;
        }
        frameCount++;
        std::cout << frameCount << std::endl;
    }
    delete camera0;
    delete camera1;
	return 0;
}

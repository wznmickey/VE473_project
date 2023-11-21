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
    //Car_Detection detect;
    //Init distcalc
    Detection detection;
    int frameCount = 0;
    int fourcc = cv::VideoWriter::fourcc('X', 'V', 'I', 'D');
    double fps = 30.0;
    cv::Size frameSize(640, 480);
    cv::VideoWriter out("output.avi", fourcc, fps, frameSize);
    distcalcinit();

	while (flag) {
        cv::Mat frame;
        for (int i = 0; i < 1 ; i++)
        {
            frame = camera.take_pic();
        }
        if (frame.empty()) break;
        cv::Mat frameL = camera.get_pic(LEFT);
        //vector<cv::Rect2d> roi_vec = detect.callNetworks(frameL);
        detection.detect( frameL );
        vector<cv::Rect2i> roi_vec = detection.get( );

        if(!roi_vec.empty())
        {
            for(cv::Rect2i roi: roi_vec)
            {
                double dist = calculateDistance(seperatePhoto(camera.get_pic(COMPLETE)),roi);
                std::cout << "Distance: " << dist << std::endl;
                detection.drawRectText(roi, std::to_string(dist).substr(0,4));
            }
            detection.ImgSave("/home/pi/VE473_project/img/result.png");

            std::cout << "Writing " << frameCount << "-th frame...\n";
            frameCount +=1;
            out.write(detection.getImage());
            // char q;
            // std::cin.get(q);
            // if (q == 'a') return 0;
            // sleep(2);
        }
        //camera.save_pic(LEFT, "imageL.jpg");
        //camera.save_pic(RIGHT, "imageR.jpg");
	}
    out.release();
	return 0;
}
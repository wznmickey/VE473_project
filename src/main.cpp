#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <cstdlib>
#include <signal.h>
#include <fstream>
#include "Camera.h"
#include "config.h"

bool flag = true;
void signal_callback_handler(int signum) {
   flag = false;
}

cfg::CONFIG config;

int main()
{
    signal(SIGINT, signal_callback_handler);
    Camera camera;
	while (flag) {
        camera.take_pic();
        camera.save_pic(LEFT, "imageL.jpg");
        camera.save_pic(RIGHT, "imageR.jpg");
	}
	return 0;
}
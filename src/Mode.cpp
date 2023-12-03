#include "Mode.h"

//Return False if error occurs and needs to stop.
bool EfficiencyMode(
    Camera& camera0, 
    Camera& camera1, 
    Gyro& gyro, 
    Buzzer& buzzer, 
    DistanceCalc& distances, 
    Detection& detection, 
    Mode& mode)
    {
        if (gyro.getAlertFlag()) mode = PERFORMANCE;

        static struct timeval startTime;
        static struct timeval endTime;
        static float EfficiencyTime = -1;
        if (EfficiencyTime < 0) gettimeofday(&startTime, NULL);

        static short whichcam = -1;
        whichcam = (whichcam+1)%2;
        Camera * camera;
        switch (whichcam)
        {
        case 0:
            camera = &camera0;
            break;
        case 1:
            camera = &camera1;
            break;
        default:
            return false;
            break;
        }
        
        static cv::Mat frame;
        frame = camera->take_pic();
        if (frame.empty()) return false;

        std::vector<cv::Mat> separated_photo;
        vector<cv::Rect2i> roi_vec;
        static cv::Mat frameL;
        static cv::Mat frameR;
        double minDist = DBL_MAX;
        double dist = 0;

        frameL = camera->get_pic(LEFT);
        frameR = camera->get_pic(RIGHT);
        detection.detect(frameL);

        separated_photo.emplace_back(frameL);
        separated_photo.emplace_back(frameR);
        distances.calculateMap(separated_photo);

        roi_vec = detection.get();
        
        if(!roi_vec.empty())
        {
            for(cv::Rect2i roi: roi_vec)
            {
                dist = distances.calculateDistance(separated_photo, roi);
                std::cout << "Distance of cam: " << dist << std::endl;
                minDist = min(dist,minDist);
                detection.drawRectText(roi,std::to_string(dist).substr(0, 4));
            }
            string filename = "/home/pi/www/show.jpg";
            detection.ImgSave(filename);
        }
        // TODO: Add switch to performance according to the gyro sensor
        // Distance is too small
        if(minDist < 1.0) 
        {
            buzzer.buzz(500);
            mode = PERFORMANCE;
        }

        if (EfficiencyTime < 0)
        {
            gettimeofday(&endTime, NULL);
            EfficiencyTime = timeDiff(startTime, endTime);
        } 
        usleep(int(1000*EfficiencyTime));
        return true;
    }

bool PerformanceMode(
    Camera& camera0, 
    Camera& camera1, 
    Gyro& gyro, 
    Buzzer& buzzer, 
    DistanceCalc& distances, 
    Detection& detection, 
    Mode& mode)
    {
        static cv::Mat frame0;
        static cv::Mat frame1;
        static cv::Mat frameL;
        static cv::Mat frameR;
        static bool Cam0NoRoi = false;
        static bool Cam1NoRoi = false;

        // gyro->readGyro();

        std::vector<cv::Mat> separated_photo;
        vector<cv::Rect2i> roi_vec;
        
        double minDist = DBL_MAX;
        double dist = 0;
        if(!Cam0NoRoi)
        {
            frame0 = camera0.take_pic();
            if(frame0.empty()) return false;
            frameL = camera0.get_pic(LEFT);
            frameR = camera0.get_pic(RIGHT);
            detection.detect(frameL);

            separated_photo.emplace_back(frameL);
            separated_photo.emplace_back(frameR);
            distances.calculateMap(separated_photo);

            roi_vec = detection.get();
            if(!roi_vec.empty())
            {
                for(cv::Rect2i roi: roi_vec)
                {
                    dist = distances.calculateDistance(separated_photo, roi);
                    std::cout << "Distance of cam0: " << dist << std::endl;
                    minDist = min(dist,minDist);
                    detection.drawRectText(roi,std::to_string(dist).substr(0, 4));
                }
                string filename = "/home/pi/www/show.jpg";
                detection.ImgSave(filename);
            }
            else Cam0NoRoi = true;
        }
        else
        {
            Cam0NoRoi = false;
            if(Cam1NoRoi) //Both roi empty
            {
                mode = EFFICIENCY;
            }
        }
        if(!Cam1NoRoi)
        {
            frame1 = camera1.take_pic();
            if(frame1.empty()) return false;

            frameL = camera1.get_pic(LEFT);
            frameR = camera1.get_pic(RIGHT);
            // gyro->readGyro();
            detection.detect(frameL);

            separated_photo.clear();
            separated_photo.emplace_back(frameL);
            separated_photo.emplace_back(frameR);
            distances.calculateMap(separated_photo);

            roi_vec = detection.get();

            if(!roi_vec.empty())
            {
                for(cv::Rect2i roi: roi_vec)
                {
                    dist = distances.calculateDistance(separated_photo, roi);
                    std::cout << "Distance of cam1: " << dist << std::endl;
                    minDist = min(dist,minDist);
                    detection.drawRectText(roi,std::to_string(dist).substr(0, 4));
                }
                string filename = "/home/pi/www/show.jpg";
                detection.ImgSave(filename);
            }
            else Cam1NoRoi = true;
        }
        else
        {
            Cam1NoRoi = false;
            if(Cam0NoRoi) //Both roi empty
            {
                mode = EFFICIENCY;
            }
        }
        
        if(mode  == EFFICIENCY) printf(RED "Switch to Efficiency mode\n" NONE);

        if(minDist < 1.0) buzzer.buzz(500);
        return true;
    }
#ifndef CAR_H
#define CAR_H

#include <fstream>
#include <sstream>
#include <iostream>

#include <opencv2/imgproc.hpp>

#define START_ACTIVITY_COUNTER 50

/**
 * @brief describe the detected car on the image
*/
class Car
{
    private:
		bool mIsActive;
	
    public:
		int mActivityCounter;
		long id;
		static long globalId;
		int centerIndex;
		cv::Point center;
		
		bool IsActive();
		Car(cv::Point&);
		
		void filter();
		void countDown();

};


#endif // car.h

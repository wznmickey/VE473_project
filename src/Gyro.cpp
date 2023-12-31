#include "Gyro.h"

bool Gyro::GyroTurn(void)
{
    float ax, ay, az, gr, gp, gy; //Variables to store the accel, gyro and angle values

	sleep(1); //Wait for the MPU6050 to stabilize

	//Read the current yaw angle
	this->gyroscope.calc_yaw = true;

	//while(true) {
		//gyroscope.getAngle(0, &gr);
		//gyroscope.getAngle(1, &gp);
		//gyroscope.getAngle(2, &gy);
		this->gyroscope.getGyro(&gr, &gp, &gy);
		std::cout << "Gyroscope Readings: X: " << gr << ", Y: " << gp << ", Z: " << gy << "\n";
		if (gr > 200 || gy > 200 || gr < -200 ||  gy < -200) {
		// Check accelerator to enter Performance mode
            		std::cout << "Performance mode" << "\n"; // Enter performance mode
            		
            		//auto start = chrono::high_resolution_clock::now();
            		
					// Performance mode last for 3 seconds
            		sleep(3);
					/*
            		while (true) 
					{
						auto end = chrono::high_resolution_clock::now();
						chrono::duration<double> elapsed = end - start;
						if (elapsed.count() >= 3.0) {
							cout << "Go normal" << "\n"; // Back to efficency mode
							break;
						}
        			} 
					*/
        			//continue; // Go back to the main loop
        	}
        
		usleep(100000); //0.1sec
	//}
    /*
        //Get the current accelerometer values
        gyroscope.getAccel(&ax, &ay, &az);
        cout << "Accelerometer Readings: X: " << ax << ", Y: " << ay << ", Z: " << az << "\n";
    */
	return false;
}

void Gyro::setAlertThreshold(int th) {
	if (th >= 2) this->alertThreshold = th;
	else this->alertThreshold = 1;
}

/**
 * @brief return alert flag, if true then clean the flag
 * @return bool
*/
bool Gyro::getAlertFlag(void) {
	if (alertFlag) {
		alertFlag = false;
		return true;
	}
	return false;
};

bool Gyro::readGyro(bool verbose) {
	float accx, accy, accz;
	this->gyroscope.getGyro(&accx, &accy, &accz);
	float accelerationMagnitude = sqrt(accx * accx + accy * accy + accz * accz);
	if (verbose)
		std::cout << "acc magnitude: " << accelerationMagnitude << std::endl;
	if (accelerationMagnitude >= joggleTolerance) {
		this->alertCount += 1;
		alertness = 6;
		if (alertCount >= alertThreshold) {
			alertFlag = true;
			alertCount = 0;
			alertness = 0;
		}
	} else {
		alertness--;
		if (alertness==0) alertCount = 0;
	}
	return true;
}

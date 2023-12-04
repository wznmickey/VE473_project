#include "MPU6050.h"
#include <chrono>

using namespace std;

MPU6050 gyroscope(0x68);

bool GyroTurn(MPU6050& gyroscope) {
	float ax, ay, az, gr, gp, gy; //Variables to store the accel, gyro and angle values

	sleep(1); //Wait for the MPU6050 to stabilize

	//Read the current yaw angle
	gyroscope.calc_yaw = true;

	while(true) {
		//gyroscope.getAngle(0, &gr);
		//gyroscope.getAngle(1, &gp);
		//gyroscope.getAngle(2, &gy);
		gyroscope.getGyro(&gr, &gp, &gy);
		cout << "Gyroscope Readings: X: " << gr << ", Y: " << gp << ", Z: " << gy << "\n";
		if (gr > 200 || gy > 200 || gr < -200 ||  gy < -200) {
		// Check accelerator to enter Performance mode
            		cout << "Performance mode" << "\n"; // Enter performance mode
            		
            		auto start = chrono::high_resolution_clock::now();
            		
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
        			continue; // Go back to the main loop
        	}
        
		usleep(100000); //0.25sec
	}
/*
	//Get the current accelerometer values
	gyroscope.getAccel(&ax, &ay, &az);
	cout << "Accelerometer Readings: X: " << ax << ", Y: " << ay << ", Z: " << az << "\n";
*/
	return 0;
}

//int main() {
//	sleep(1);
//	GyroTurn(gyroscope);
//}

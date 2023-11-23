#include <MPU6050.h>

using namespace std;

MPU6050 device(0x68);

int main() {
	float ax, ay, az, gr, gp, gy; //Variables to store the accel, gyro and angle values

	sleep(1); //Wait for the MPU6050 to stabilize


	//Read the current yaw angle
	device.calc_yaw = true;
/*
	for (int i = 0; i < 40; i++) {
        device.getGyro(&gr, &gp, &gy);
        cout << "Gyroscope Readings: X: " << gr << ", Y: " << gp << ", Z: " << gy << "\n";
        if (gr > 100) {
            // Enter performance mode
            cout << "Handlebar turning too fast!" << "\n";
        }
		usleep(1000000); //1 sec
	}
 */
    while (true) {
        device.getGyro(&gr, &gp, &gy);
        cout << "Gyroscope Readings: X: " << gr << ", Y: " << gp << ", Z: " << gy << "\n";
        if (gr > 50) {
            cout << "Turning" << "\n";
            // Enter performance mode
        } else {
            cout << "Normal" << "\n";
            // Efficiency mode
        }
        usleep(1000000); //1 sec
    }


/*
    //Get the current accelerometer values
	device.getAccel(&ax, &ay, &az);
	std::cout << "Accelerometer Readings: X: " << ax << ", Y: " << ay << ", Z: " << az << "\n";

	//Get the current gyroscope values

*/
	return 0;
}



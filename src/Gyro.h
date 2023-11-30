#ifndef GYRO_H
#define GYRO_H

#include "GyroTest/MPU6050.h"
#include <chrono>

class Gyro
{
    private:
        MPU6050 gyroscope = MPU6050(0x68);
        int alertThreshold = 2;
        int alertCount = 0;
        bool alertFlag = false;
        float joggleTolerance = 50.0;
    public:
        Gyro(){};
        ~Gyro(){};
        bool GyroTurn(void);
        void setAlertThreshold(int th);
        bool getAlertFlag(void);
        bool readGyro(void);
        void setjoggleTolerance(float j) {this->joggleTolerance = j;};
};

#endif

#ifndef GYRO_H
#define GYRO_H

#include "GyroTest/MPU6050.h"
#include <chrono>

class Gyro
{
    private:
        MPU6050 gyroscope = MPU6050(0x68);
    public:
        Gyro(){};
        ~Gyro(){};
        bool GyroTurn(void);
};

#endif

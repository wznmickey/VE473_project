#ifndef MODE_H
#define MODE_H

#include "Camera.h"
#include "config.h"
#include "car.h"
#include "car_detection.h"
#include "distance.h"
#include "Detection.h"
#include "Gyro.h"
#include "buzzer.h"
#include "Mode.h"
#include <climits>

enum Mode
{
    EFFICIENCY, PERFORMANCE
};

bool EfficiencyMode(Camera*, Camera*, Gyro*, Buzzer*, DistanceCalc*, Detection*, Mode*);
bool PerformanceMode(Camera*, Camera*, Gyro*, Buzzer*, DistanceCalc*, Detection*, Mode*);

#endif
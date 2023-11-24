#ifndef BUZZER_H
#define BUZZER_H

#include <iostream>
#include <wiringPi.h> // Library for Raspberry Pi GPIO access
#include <unistd.h>   // Library for sleep function

bool buzzer(unsigned long int milisecond);

#endif
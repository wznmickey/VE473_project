#ifndef BUZZER_H
#define BUZZER_H

#include <iostream>
#include <wiringPi.h> // Library for Raspberry Pi GPIO access
#include <unistd.h>   // Library for sleep function
#include <thread>

void work(Buzzer * buzzer);

class Buzzer
{
    public:
        Buzzer();
        ~Buzzer();
        bool buzz(unsigned long int milisecond);
    private:
        int buzzer_pin;
        bool isrunning = true;
        unsigned long int buzztimems;
        std::thread buzzthread(work,this);
        
};

#endif
#ifndef BUZZER_H
#define BUZZER_H

#include <iostream>
#include <wiringPi.h> // Library for Raspberry Pi GPIO access
#include <unistd.h>   // Library for sleep function
#include <thread>
#include <unistd.h>
#include <functional>
#include <mutex>



class Buzzer
{
    public:
        Buzzer();
        ~Buzzer();
        void buzz(unsigned long int milisecond);
        void work();
    private:
        int buzzer_pin;
        bool isrunning = true;
        unsigned long int buzztimems = 0;
        std::thread buzzthread;
        std::mutex mtxtime;
        std::mutex mtxwork;
};

#endif
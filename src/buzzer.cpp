#include "buzzer.h"

const static int HERZ = 1000;


Buzzer::Buzzer()
{
    // Set up wiringPi
    wiringPiSetupGpio();

    // Set the GPIO pin number
    this->buzzer_pin = 18;

    // Set up the GPIO pin as an output
    pinMode(buzzer_pin, OUTPUT);

    this->buzzthread = std::thread(std::bind(&Buzzer::work,this));
}

Buzzer::~Buzzer()
{
    this->isrunning = false;
    this->buzzthread.join();
}

void Buzzer::work()
{
    while(this->isrunning)
    {
        if (this->buzztimems > 0)
        {
            unsigned long int milisecond = 0;
            this->mtx.lock();
            if (this->buzztimems >= 100)
            {
                milisecond = 100;
                this->buzztimems -= 100;
            }
            else{
                milisecond = this->buzztimems;
                this->buzztimems = 0;
            }
            this->mtx.unlock();
            for(unsigned long int i = 0; i < milisecond*HERZ/1000; i++) {
                // Turn on the buzzer
                digitalWrite(buzzer_pin, HIGH);
                //delayMicroseconds(1000000/HERZ/2);  // Buzzer on for 1 second
                usleep(1000000/HERZ/2);

                // Turn off the buzzer
                digitalWrite(buzzer_pin, LOW);
                //delayMicroseconds(1000000/HERZ/2);  // Buzzer off for 1 second
                usleep(1000000/HERZ/2);
            }
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
}

void Buzzer::buzz(unsigned long int milisecond)
{
    this->mtx.lock();
    this->buzztimems = std::max(this->buzztimems,milisecond);
    this->mtx.unlock();
    // // Set up wiringPi
    // wiringPiSetupGpio();

    // // Set the GPIO pin number
    // int buzzer_pin = 18;

    // // Set up the GPIO pin as an output
    // pinMode(buzzer_pin, OUTPUT);

    // try {
    //     for(unsigned long int i = 0; i < milisecond/1000*HERZ; i++) {
    //         // Turn on the buzzer
    //         digitalWrite(buzzer_pin, HIGH);
    //         //delayMicroseconds(1000000/HERZ/2);  // Buzzer on for 1 second
    //         usleep(1000000/HERZ/2);

    //         // Turn off the buzzer
    //         digitalWrite(buzzer_pin, LOW);
    //         //delayMicroseconds(1000000/HERZ/2);  // Buzzer off for 1 second
    //         usleep(1000000/HERZ/2);
    //     }
    // } catch (...) {
    //     // Cleanup GPIO
    //     digitalWrite(buzzer_pin, LOW);
    //     pinMode(buzzer_pin, INPUT);
    //     std::cerr << "Error occurred." << std::endl;
    //     return 1;
    // }

    // return 0;
}
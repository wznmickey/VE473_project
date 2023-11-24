#include "buzzer.h"

const static int HERZ = 1000;

bool buzzer(unsigned long int milisecond) {
    // Set up wiringPi
    wiringPiSetupGpio();

    // Set the GPIO pin number
    int buzzer_pin = 18;

    // Set up the GPIO pin as an output
    pinMode(buzzer_pin, OUTPUT);

    try {
        for(unsigned long int i = 0; i < milisecond/1000*HERZ; i++) {
            // Turn on the buzzer
            digitalWrite(buzzer_pin, HIGH);
            //delayMicroseconds(1000000/HERZ/2);  // Buzzer on for 1 second
            usleep(1000000/HERZ/2);

            // Turn off the buzzer
            digitalWrite(buzzer_pin, LOW);
            //delayMicroseconds(1000000/HERZ/2);  // Buzzer off for 1 second
            usleep(1000000/HERZ/2);
        }
    } catch (...) {
        // Cleanup GPIO
        digitalWrite(buzzer_pin, LOW);
        pinMode(buzzer_pin, INPUT);
        std::cerr << "Error occurred." << std::endl;
        return 1;
    }

    return 0;
}

#include <iostream>
#include <wiringPi.h> // Library for Raspberry Pi GPIO access
#include <unistd.h>   // Library for sleep function

bool buzzer() {
    // Set up wiringPi
    wiringPiSetupGpio();

    // Set the GPIO pin number
    int buzzer_pin = 18;

    // Set up the GPIO pin as an output
    pinMode(buzzer_pin, OUTPUT);

    try {
        while (true) {
            // Turn on the buzzer
            digitalWrite(buzzer_pin, HIGH);
            delay(1000);  // Buzzer on for 1 second

            // Turn off the buzzer
            digitalWrite(buzzer_pin, LOW);
            delay(1000);  // Buzzer off for 1 second
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

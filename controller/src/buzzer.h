#ifndef BUZZER_H
#define BUZZER_H

#include <Arduino.h>

class Buzzer {
public:
    Buzzer(pin_size_t pinNumber) : pinNumber{pinNumber} {
        pinMode(pinNumber, OUTPUT);
    }

    void beepInitialized() {
        tone(pinNumber, 880, 25);
    }
private:
    pin_size_t pinNumber;
};

#endif

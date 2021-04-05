#ifndef PIN_H
#define PIN_H

#include <Arduino.h>

class Pin {
public:
    Pin(pin_size_t number) : number{number} {};
    Pin(pin_size_t number, PinMode mode) : Pin(number) {
        pinMode(number, mode);
    };
    Pin(pin_size_t number, PinMode mode, PinStatus initialStatus) : Pin(number, mode) {
        digitalWrite(number, initialStatus);
    }

    void setStatus(PinStatus status) {
        digitalWrite(number, status);
    }

    void setAnalogValue(int value) {
        analogWrite(number, value);
    }
private:
    pin_size_t number;
};

#endif

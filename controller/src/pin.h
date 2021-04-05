/*
 * Object-oriented pin manipulation.
 */

#ifndef PIN_H
#define PIN_H

#include <Arduino.h>

class Pin {
public:
    const pin_size_t number;

    Pin(pin_size_t number) : number{number} {};
    Pin(pin_size_t number, PinMode mode) : Pin(number) {
        pinMode(number, mode);
    };
    Pin(pin_size_t number, PinMode mode, PinStatus initialStatus) : Pin(number, mode) {
        digitalWrite(number, initialStatus);
    }

    PinStatus getStatus() const {
        return digitalRead(number);
    }

    void attachInterrupt(PinStatus mode, void (*userFunc)(void)) const {
        ::attachInterrupt(digitalPinToInterrupt(number), userFunc, mode);
    }

    void setStatus(PinStatus status) const {
        digitalWrite(number, status);
    }

    void setAnalogValue(int value) const {
        analogWrite(number, value);
    }
};

#endif

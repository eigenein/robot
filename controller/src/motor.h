/*
 * Object-oriented motor control.
 */

#ifndef MOTOR_H
#define MOTOR_H

#include <Arduino.h>

#include "pin.h"

class Motor {
public:
    Motor(const pin_size_t pin1, const pin_size_t pin2, const pin_size_t pinPWM) :
        pin1{Pin(pin1, OUTPUT, LOW)},
        pin2{Pin(pin2, OUTPUT, LOW)},
        pinPWM{Pin(pinPWM, OUTPUT, LOW)} {
    }

    void setSpeed(const int speed) {
        if (speed == 0) {
            pin1.setStatus(LOW);
            pin2.setStatus(LOW);
        } else if (speed > 0) {
            pin1.setStatus(LOW);
            pin2.setStatus(HIGH);
            pinPWM.setAnalogValue(speed);
        } else {
            pin2.setStatus(LOW);
            pin1.setStatus(HIGH);
            pinPWM.setAnalogValue(-speed);
        }
    }
private:
    const Pin pin1;
    const Pin pin2;
    const Pin pinPWM;
};
#endif

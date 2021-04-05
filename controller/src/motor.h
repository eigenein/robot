#ifndef MOTOR_H
#define MOTOR_H

#include <Arduino.h>

static const float MIN_FEASIBLE_TICK_SPEED = 4.0f;
static const unsigned long MAX_TICK_INTERVAL_MICROS = 1000000;

class Motor {
public:
    // Proportional gain.
    float kp = 1.0f;

    float targetTickSpeed = 0;

    Motor(const pin_size_t pin1, const pin_size_t pin2, const pin_size_t pinPWM)
        : pin1{pin1}, pin2{pin2}, pinPWM{pinPWM}
    {
        digitalWrite(pin1, LOW);
        pinMode(pin1, OUTPUT);
        digitalWrite(pin2, LOW);
        pinMode(pin2, OUTPUT);
        digitalWrite(pinPWM, LOW);
        pinMode(pinPWM, OUTPUT);
    }

    void setSpeed(const int speed) {
        if (speed == 0) {
            digitalWrite(pin1, LOW);
            digitalWrite(pin2, LOW);
        } else if (speed > 0) {
            digitalWrite(pin1, LOW);
            digitalWrite(pin2, HIGH);
            analogWrite(pinPWM, speed);
        } else {
            digitalWrite(pin2, LOW);
            digitalWrite(pin1, HIGH);
            analogWrite(pinPWM, -speed);
        }
    }
private:
    const pin_size_t pin1;
    const pin_size_t pin2;
    const pin_size_t pinPWM;
};
#endif

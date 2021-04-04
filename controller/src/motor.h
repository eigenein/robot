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
        : pin1{pin1}, pin2{pin2}, pinPWM{pinPWM} {}

    void initializePins() {
        digitalWrite(pin1, LOW);
        pinMode(pin1, OUTPUT);
        digitalWrite(pin2, LOW);
        pinMode(pin2, OUTPUT);
        digitalWrite(pinPWM, LOW);
        pinMode(pinPWM, OUTPUT);
    }

    float getKi() const {
        return ki;
    }

    void setKi(float ki) {
        this->ki = ki;
        this->pidIntegral = 0;
    }

    float getKd() const {
        return kd;
    }

    void setKd(float kd) {
        this->kd = kd;
        this->previousPIDError = 0;
    }

    float getCurrentTickSpeed() const {
        if ((micros() - lastLowTickMicros) > MAX_TICK_INTERVAL_MICROS) {
            // We haven't heard from the sensor for too long, we're probably stopped.
            return 0.0f;
        }
        return lastHighTickSpeed + lastLowTickSpeed;
    }

    // Callback for the rotary encoder state change.
    void onRotaryTickInterrupt(const PinStatus pinStatus) {
        const unsigned long nowMicros = micros();
        if (pinStatus == HIGH) {
            lastHighTickSpeed = 1000000.0f / (nowMicros - lastHighTickMicros);
            lastHighTickMicros = nowMicros;
        } else {
            lastLowTickSpeed = 1000000.0f / (nowMicros - lastLowTickMicros);
            lastLowTickMicros = nowMicros;
        }
    }

    // Update the controlling PID signal.
    void update(const unsigned long elapsedMicros) {
        updatePID(elapsedMicros);
        updatePWM();
    }

    float getCurrentPIDSignal() const {
        return currentPIDSignal;
    }
private:
    const pin_size_t pin1;
    const pin_size_t pin2;
    const pin_size_t pinPWM;

    unsigned long lastHighTickMicros = 0;
    float lastHighTickSpeed = 0;
    unsigned long lastLowTickMicros = 0;
    float lastLowTickSpeed = 0;

    // Integral gain.
    float ki = 2.0f;
    // Differential gain.
    float kd = 0.1f;

    float currentPIDSignal = 0;
    float pidIntegral = 0;
    float previousPIDError = 0;

    // Update the PID controller state.
    void updatePID(unsigned long elapsedMicros) {
        const float elapsed = elapsedMicros / 1000000.f;

        // The PID controller only controls the PWM, not the motor rotation direction, hence the `fabs`.
        // Also see: https://en.wikipedia.org/wiki/PID_controller#Pseudocode
        const float error = fabs(targetTickSpeed) - getCurrentTickSpeed();
        const float proportional = error;
        pidIntegral += error * elapsed;
        pidIntegral = constrain(pidIntegral, -255.0f, 255.0f);
        const float derivative = (error - previousPIDError) / elapsed;
        previousPIDError = error;
        currentPIDSignal = kp * proportional + ki * pidIntegral + kd * derivative;
        currentPIDSignal = constrain(currentPIDSignal, 0.0f, 255.0f);
    }

    // Update PWM signal based on the current PID signal.
    void updatePWM() {
        const int pwmValue = (int)currentPIDSignal;
        // Rotation direction is still based on `targetTickSpeed`.
        analogWrite(pinPWM, pwmValue);
        if ((pwmValue == 0) || (fabs(targetTickSpeed) < MIN_FEASIBLE_TICK_SPEED)) {
            digitalWrite(pin1, LOW);
            digitalWrite(pin2, LOW);
        } else if (targetTickSpeed > 0.0f) {
            digitalWrite(pin1, LOW);
            digitalWrite(pin2, HIGH);
        } else {
            digitalWrite(pin2, LOW);
            digitalWrite(pin1, HIGH);
        }
    }
};
#endif

/*
 * Motor rotary encoders tracking.
 */

#ifndef ROTARY_H
#define ROTARY_H

#include <Arduino.h>

#include "elapsed.h"

static const unsigned long DEBOUNCE_INTERVAL_MICROS = 10000;

class Rotary {
public:
    void onInterrupt() {
        if (elapsed.elapsedMicros() > DEBOUNCE_INTERVAL_MICROS) {
            speed = 1.0f / elapsed.elapsedSecs();
            elapsed.reset();
        }
    }

    float getSpeed() const {
        const float estimate = getCurrentEstimate();
        return min(estimate, speed);
    }
private:
    Elapsed elapsed;
    float speed = 0.0f;

    float getCurrentEstimate() const {
        return 1.0f / elapsed.elapsedSecs();
    }
};

#endif

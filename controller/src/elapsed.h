/*
 * Utility class to measure elapsed time.
 */

#ifndef ELAPSED_H
#define ELAPSED_H

#include <Arduino.h>

class Elapsed {
public:
    Elapsed() {
        reset();
    }

    void reset() {
        startMicros = micros();
    }

    unsigned long elapsedMicros() const {
        return micros() - startMicros;
    }

    float elapsedSecs() const {
        return elapsedMicros() / 1000000.0f;
    }
private:
    unsigned long startMicros = 0;
};

#endif

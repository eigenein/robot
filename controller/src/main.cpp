#include <Arduino.h>

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>
#include <Ticker.h>

// Fundamental constants.
// -------------------------------------------------------------------------------------------------

const unsigned int TELEMETRY_INTERVAL_MILLIS = 500;
const unsigned int COMPASS_INTERVAL_MILLIS = 1000 / 15; // Default readings rate is 15Hz.

// Adjusted experimentally.
// Warning: dependent on `COMPASS_INTERVAL_MILLIS`.
const float COMPASS_SMOOTHING_FACTOR = 0.3f;

// Peripherals.
// -------------------------------------------------------------------------------------------------
Adafruit_HMC5883_Unified compass = Adafruit_HMC5883_Unified();

// Tickers.
// -------------------------------------------------------------------------------------------------

void printTelemetry();
void readCompass();

Ticker telemetryTicker(printTelemetry, TELEMETRY_INTERVAL_MILLIS);
Ticker readCompassTicker(readCompass, COMPASS_INTERVAL_MILLIS);

// Current state.
// -------------------------------------------------------------------------------------------------

float compassX = 0.0f;
float compassY = 0.0f;

// Initializers.
// -------------------------------------------------------------------------------------------------

void initializePins() {
    pinMode(LED_BUILTIN, OUTPUT);
}

void initializeSerial() {
    Serial.begin(9600);
    while (!Serial);
}

void initializeCompass() {
    if (!compass.begin()) {
        Serial.println("Error: no HMC5883 detected.");
        while(1);
    }
}

void startTickers() {
    telemetryTicker.start();
    readCompassTicker.start();
}

void setup() {
    initializePins();
    initializeSerial();
    initializeCompass();
    startTickers();
}

// Functionality.
// -------------------------------------------------------------------------------------------------

void printTelemetry() {
    Serial.print("Compass: ");
    Serial.print(compassX);
    Serial.print(" ");
    Serial.print(compassY);
    Serial.println();
}

void readCompass() {
    static float minX = 0.0f, maxX = 0.0f, minY = 0.0f, maxY = 0.0f;
    static float filteredX = 0.0f;
    static float filteredY = 0.0f;

    sensors_event_t event; 
    compass.getEvent(&event);

    filteredX += (event.magnetic.x - filteredX) * COMPASS_SMOOTHING_FACTOR;
    filteredY += (event.magnetic.y - filteredY) * COMPASS_SMOOTHING_FACTOR;

    minX = min(minX, filteredX);
    maxX = max(maxX, filteredX);
    minY = min(minY, filteredY);
    maxY = max(maxY, filteredY);

    float dX = filteredX - (maxX + minX) / 2.0f;
    float dY = filteredY - (maxY + minY) / 2.0f;
    float magnitude = sqrt(dX * dX + dY * dY);

    compassX = dX / magnitude;
    compassY = dY / magnitude;
}

// The Loop.
// -------------------------------------------------------------------------------------------------

void loop() {
    telemetryTicker.update();
    readCompassTicker.update();
}

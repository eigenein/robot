#include <Arduino.h>

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>
#include <Ticker.h>

// Fundamental constants.
// -------------------------------------------------------------------------------------------------

static const unsigned int PRINT_TELEMETRY_INTERVAL_MILLIS = 500;
static const unsigned int READ_COMPASS_INTERVAL_MILLIS = 1000 / 15; // Default readings rate is 15Hz.

static const float COMPASS_SMOOTHING_FACTOR = 0.3f;

// Peripherals and pins.
// -------------------------------------------------------------------------------------------------
static const unsigned int PIN_ROTARY_LEFT = 20;
static const unsigned int PIN_ROTARY_RIGHT = 21;

Adafruit_HMC5883_Unified compass = Adafruit_HMC5883_Unified();

// Tickers.
// -------------------------------------------------------------------------------------------------

void printTelemetry();
void readCompass();

Ticker telemetryTicker(printTelemetry, PRINT_TELEMETRY_INTERVAL_MILLIS);
Ticker readCompassTicker(readCompass, READ_COMPASS_INTERVAL_MILLIS);


// Current state.
// -------------------------------------------------------------------------------------------------

static float compassX = 0.0f;
static float compassY = 0.0f;

static float positionTicksX = 0.0f;
static float positionTicksY = 0.0f;

// Initializers.
// -------------------------------------------------------------------------------------------------

void incrementRotary();

void enableRotaryInterrupts() {
    attachInterrupt(digitalPinToInterrupt(PIN_ROTARY_LEFT), incrementRotary, CHANGE);
    attachInterrupt(digitalPinToInterrupt(PIN_ROTARY_RIGHT), incrementRotary, CHANGE);
}

void initializePins() {
    pinMode(PIN_ROTARY_LEFT, INPUT);
    pinMode(PIN_ROTARY_RIGHT, INPUT);

    enableRotaryInterrupts();

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

    Serial.print(" | ");

    Serial.print("Position: ");
    Serial.print(positionTicksX);
    Serial.print(" ");
    Serial.print(positionTicksY);

    Serial.println();
}

void readCompass() {
    // Globally measured min's and max's:
    static float minX = 0.0f, maxX = 0.0f, minY = 0.0f, maxY = 0.0f;
    
    // Here we keep the current filtered yet raw measurements:
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

    // Apply the offsets and normalize the magnetic vector:
    float unnormalizedX = filteredX - (maxX + minX) / 2.0f;
    float unnormalizedY = filteredY - (maxY + minY) / 2.0f;
    float magnitude = sqrt(unnormalizedX * unnormalizedX + unnormalizedY * unnormalizedY);
    compassX = unnormalizedX / magnitude;
    compassY = unnormalizedY / magnitude;
}

void incrementRotary() {
    // TODO: wheel rotation direction.
    positionTicksX += 0.5f * compassX;
    positionTicksY += 0.5f * compassY;
}

// The Loop.
// -------------------------------------------------------------------------------------------------

void loop() {
    telemetryTicker.update();
    readCompassTicker.update();
}

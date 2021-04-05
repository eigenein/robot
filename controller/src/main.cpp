#include <math.h>

#include <Arduino.h>

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <Ticker.h>

#include "buzzer.h"
#include "motor.h"
#include "rotary.h"

// -------------------------------------------------------------------------------------------------
// Custom types.
// -------------------------------------------------------------------------------------------------

// Ticker interval, milliseconds.
typedef uint32_t millis_t;

// Wheel rotation speed, ticks per second.
typedef float tick_speed_t;

// -------------------------------------------------------------------------------------------------
// Fundamental constants.
// -------------------------------------------------------------------------------------------------

static const size_t MAX_CONSOLE_INPUT_LENGTH = 40;

// -------------------------------------------------------------------------------------------------
// Peripherals and pins.
// -------------------------------------------------------------------------------------------------

static const Buzzer buzzer = Buzzer(17);

static const Motor leftMotor(8, 7, 10);
static const Motor rightMotor(4, 2, 9);

static const Adafruit_BNO055 orientationSensor = Adafruit_BNO055(-1, 0x29);

static const Pin leftRotaryPin = Pin(21, INPUT);
static const Pin rightRotaryPin = Pin(20, INPUT);

static const Rotary leftRotaryEncoder;
static const Rotary rightRotaryEncoder;

// -------------------------------------------------------------------------------------------------
// Forward declarations.
// -------------------------------------------------------------------------------------------------

void initializeSerial();
void initializePins();
void initializeOrientationSensor();
void startTickers();

void printTelemetry();
void readConsole();
void readOrientationSensor();

void handleConsoleInput(const char[], const size_t);

void onLeftRotaryInterrupt();
void onRightRotaryInterrupt();

// -------------------------------------------------------------------------------------------------
// Tickers.
// Don't forget to call `start` and `update` on them.
// -------------------------------------------------------------------------------------------------

Ticker printTelemetryTicker(printTelemetry, (millis_t)500);
Ticker readConsoleTicker(readConsole, (millis_t)100);
Ticker readOrientationSensorTicker(readOrientationSensor, (millis_t)50);

// -------------------------------------------------------------------------------------------------
// Current state.
// -------------------------------------------------------------------------------------------------

static bool isTelemetryEnabled = true;

static sensors_event_t orientation, acceleration;

// -------------------------------------------------------------------------------------------------
// Arduino setup and loop.
// -------------------------------------------------------------------------------------------------

void setup() {
    initializeSerial();

    Serial.println("└[∵]┘ Hi! Press <Enter> to enter the CLI.");

    initializePins();
    initializeOrientationSensor();
    startTickers();

    Serial.println("└[∵]┐ Initialization delay…");
    delay(100); // TODO: check what kind of delay is actually needed by BNO055.
    Serial.println("└[∵]┘ Initialized.");
    buzzer.beepInitialized();
}

void loop() {
    printTelemetryTicker.update();
    readConsoleTicker.update();
    readOrientationSensorTicker.update();
}

// -------------------------------------------------------------------------------------------------
// Initializers.
// -------------------------------------------------------------------------------------------------

void initializePins() {
    pinMode(LED_BUILTIN, OUTPUT);

    // It works better with either `RISING` or `FALLING`, but not `CHANGE`.
    leftRotaryPin.attachInterrupt(RISING, onLeftRotaryInterrupt);
    rightRotaryPin.attachInterrupt(RISING, onRightRotaryInterrupt);
}

void initializeSerial() {
    Serial.begin(115200);
    while (!Serial);
}

void initializeOrientationSensor() {
    if (!orientationSensor.begin()) {
        Serial.println("┌[∵]┐ Error: no BNO055 detected.");
        while (1);
    }
}

void startTickers() {
    printTelemetryTicker.start();
    readConsoleTicker.start();
    readOrientationSensorTicker.start();
}

// -------------------------------------------------------------------------------------------------
// Functionality.
// -------------------------------------------------------------------------------------------------

void printTelemetry() {
    Serial.print(orientation.orientation.x);
    Serial.print("° | ");
    Serial.print(leftRotaryEncoder.getSpeed(), 3);
    Serial.print(" ");
    Serial.print(rightRotaryEncoder.getSpeed(), 3);
    Serial.print(" rot/sec | TODO");
    Serial.println();
}

// Read the serial console and execute command, if needed.
void readConsole() {
    static char input[MAX_CONSOLE_INPUT_LENGTH + 1] = {'\0'};
    static size_t currentLength = 0;

    while (Serial.available() != 0) {
        char nextChar = Serial.read();
        Serial.print(nextChar);
        if (nextChar == '\n') {
            input[currentLength] = '\0';
            handleConsoleInput(input, currentLength);
            currentLength = 0;
        } else if ((nextChar >= ' ') && (currentLength < MAX_CONSOLE_INPUT_LENGTH)) {
            input[currentLength++] = nextChar;
        }
    }
}

// Handle user console input and execute the command.
void handleConsoleInput(const char input[], const size_t length) {
    int intArgument;
    if (length == 0) {
        isTelemetryEnabled = !isTelemetryEnabled;
        if (isTelemetryEnabled) {
            printTelemetryTicker.start();
            Serial.println("└[∵]┘ Quitting CLI. Telemetry enabled. Press <Enter> to go back to CLI.");
        } else {
            printTelemetryTicker.stop();
            Serial.println("└[∵]┘ Telemetry disabled. Entering CLI. Press <Enter> to quit.");
        }
    } else if (sscanf(input, "l%ld", &intArgument) == 1) {
        leftMotor.setSpeed(intArgument);
    } else if (sscanf(input, "r%ld", &intArgument) == 1) {
        rightMotor.setSpeed(intArgument);
    } else if (sscanf(input, "s%ld", &intArgument) == 1) {
        leftMotor.setSpeed(intArgument);
        rightMotor.setSpeed(intArgument);
    } else if (!isTelemetryEnabled) {
        Serial.print("┌[∵]┐ I don't understand: `");
        Serial.print(input);
        Serial.println("`");
    }
    if (!isTelemetryEnabled) {
        Serial.print("└[∵]┘ > ");
    }
}

void onLeftRotaryInterrupt() {
    leftRotaryEncoder.onInterrupt();
}

void onRightRotaryInterrupt() {
    rightRotaryEncoder.onInterrupt();
}

void readOrientationSensor() {
    orientationSensor.getEvent(&orientation, Adafruit_BNO055::VECTOR_EULER);
    orientationSensor.getEvent(&acceleration, Adafruit_BNO055::VECTOR_LINEARACCEL);
}

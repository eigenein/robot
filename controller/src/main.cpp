#include <math.h>

#include <Arduino.h>

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <Ticker.h>

// -------------------------------------------------------------------------------------------------
// Fundamental constants.
// -------------------------------------------------------------------------------------------------

static const unsigned int PRINT_TELEMETRY_INTERVAL_MILLIS = 500;
static const unsigned int READ_CONSOLE_INTERVAL_MILLIS = 100;
static const unsigned int READ_POSITION_SENSOR_INTERVAL_MILLIS = 50;

static const unsigned int MAX_CONSOLE_INPUT_LENGTH = 40;

// -------------------------------------------------------------------------------------------------
// Peripherals and pins.
// -------------------------------------------------------------------------------------------------

static const unsigned int PIN_ROTARY_LEFT = 20;
static const unsigned int PIN_ROTARY_RIGHT = 21;

static const unsigned int PIN_MOTOR_RIGHT_1 = 4;
static const unsigned int PIN_MOTOR_RIGHT_2 = 2;
static const unsigned int PIN_MOTOR_RIGHT_PWM = 9;

static const unsigned int PIN_MOTOR_LEFT_1 = 8;
static const unsigned int PIN_MOTOR_LEFT_2 = 7;
static const unsigned int PIN_MOTOR_LEFT_PWM = 10;

static const unsigned int PIN_BUZZER = 17;

static Adafruit_BNO055 positionSensor = Adafruit_BNO055(-1, 0x29);

// -------------------------------------------------------------------------------------------------
// Forward declarations.
// -------------------------------------------------------------------------------------------------

void initializeSerial();
void initializePins();
void initializePositionSensor();
void startTickers();

void printTelemetry();
void readCompass();
void readConsole();
void readPositionSensor();

void onLeftRotaryChange();
void onRightRotaryChange();

void controlMotor(const unsigned int, const unsigned int, const unsigned int, const int);
void controlLeftMotor(const int);
void controlRightMotor(const int);

void handleConsoleInput(const char[], const unsigned int);

// -------------------------------------------------------------------------------------------------
// Tickers.
// Don't forget to call `start` and `update` on them.
// -------------------------------------------------------------------------------------------------

Ticker printTelemetryTicker(printTelemetry, PRINT_TELEMETRY_INTERVAL_MILLIS);
Ticker readConsoleTicker(readConsole, READ_CONSOLE_INTERVAL_MILLIS);
Ticker readPositionSensorTicker(readPositionSensor, READ_POSITION_SENSOR_INTERVAL_MILLIS);

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
    initializePositionSensor();
    startTickers();

    Serial.println("└[∵]┐ Initialization delay…");
    delay(1000); // TODO: check what kind of delay is actually needed by BNO055.
    Serial.println("└[∵]┘ Initialized.");
    tone(PIN_BUZZER, 880, 50); // Initialization tone.
}

void loop() {
    printTelemetryTicker.update();
    readConsoleTicker.update();
    readPositionSensorTicker.update();
}

// -------------------------------------------------------------------------------------------------
// Initializers.
// -------------------------------------------------------------------------------------------------

void initializePins() {
    pinMode(PIN_ROTARY_LEFT, INPUT);
    pinMode(PIN_ROTARY_RIGHT, INPUT);

    attachInterrupt(digitalPinToInterrupt(PIN_ROTARY_LEFT), onLeftRotaryChange, CHANGE);
    attachInterrupt(digitalPinToInterrupt(PIN_ROTARY_RIGHT), onRightRotaryChange, CHANGE);

    pinMode(LED_BUILTIN, OUTPUT);
    
    controlLeftMotor(0);
    pinMode(PIN_MOTOR_LEFT_1, OUTPUT);
    pinMode(PIN_MOTOR_LEFT_2, OUTPUT);
    pinMode(PIN_MOTOR_LEFT_PWM, OUTPUT);

    controlRightMotor(0);
    pinMode(PIN_MOTOR_RIGHT_1, OUTPUT);
    pinMode(PIN_MOTOR_RIGHT_2, OUTPUT);
    pinMode(PIN_MOTOR_RIGHT_PWM, OUTPUT);
}

void initializeSerial() {
    Serial.begin(9600);
    while (!Serial);
}

void initializePositionSensor() {
    if (!positionSensor.begin()) {
        Serial.println("┌[∵]┐ Error: no BNO055 detected.");
        while (1);
    }
}

void startTickers() {
    printTelemetryTicker.start();
    readConsoleTicker.start();
    readPositionSensorTicker.start();
}

// -------------------------------------------------------------------------------------------------
// Functionality.
// -------------------------------------------------------------------------------------------------

void printTelemetry() {
    if (!isTelemetryEnabled) {
        return;
    }

    Serial.print("Direction: ");
    Serial.print(orientation.orientation.x);
    Serial.print(" deg");

    Serial.println();
}

// Read the serial console and execute command, if needed.
void readConsole() {
    static char input[MAX_CONSOLE_INPUT_LENGTH + 1] = {'\0'};
    static unsigned int currentLength = 0;

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
void handleConsoleInput(const char input[], const unsigned int length) {
    int motorSpeed;
    if (length == 0) {
        isTelemetryEnabled = !isTelemetryEnabled;
        if (isTelemetryEnabled) {
            Serial.println("└[∵]┘ Quitting CLI. Telemetry enabled. Press <Enter> to go back to CLI.");
        } else {
            Serial.println("└[∵]┘ Telemetry disabled. Entering CLI. Press <Enter> to quit.");
        }
    } else if (sscanf(input, "left %i", &motorSpeed) == 1) {
        controlLeftMotor(motorSpeed);
    } else if (sscanf(input, "right %i", &motorSpeed) == 1) {
        controlRightMotor(motorSpeed);
    } else {
        Serial.print("┌[∵]┐ I don't understand: `");
        Serial.print(input);
        Serial.println("`");
    }
    if (!isTelemetryEnabled) {
        Serial.print("└[∵]┘ > ");
    }
}

void onLeftRotaryChange() {
    // TODO
}

void onRightRotaryChange() {
    // TODO
}

void controlMotor(
    const unsigned int pin1,
    const unsigned int pin2,
    const unsigned int pinPWM,
    const int speed
) {
    analogWrite(pinPWM, abs(speed));
    if (speed > 0) {
        digitalWrite(pin1, LOW);
        digitalWrite(pin2, HIGH);
    } else if (speed < 0) {
        digitalWrite(pin2, LOW);
        digitalWrite(pin1, HIGH);
    } else {
        digitalWrite(pin1, HIGH);
        digitalWrite(pin2, HIGH);
    }
}

void controlLeftMotor(const int speed) {
    controlMotor(PIN_MOTOR_LEFT_1, PIN_MOTOR_LEFT_2, PIN_MOTOR_LEFT_PWM, speed);
}

void controlRightMotor(const int speed) {
    controlMotor(PIN_MOTOR_RIGHT_1, PIN_MOTOR_RIGHT_2, PIN_MOTOR_RIGHT_PWM, speed);
}

void readPositionSensor() {
    positionSensor.getEvent(&orientation, Adafruit_BNO055::VECTOR_EULER);
    positionSensor.getEvent(&acceleration, Adafruit_BNO055::VECTOR_LINEARACCEL);

    // Calculate the actual elapsed time.
    static unsigned long lastReadingMicros = micros();
    const unsigned long now = micros();
    const double elapsed = (now - lastReadingMicros) / 1000000.0;
    lastReadingMicros = now;
}

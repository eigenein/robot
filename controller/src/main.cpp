#include <Arduino.h>

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>
#include <Ticker.h>

// -------------------------------------------------------------------------------------------------
// Fundamental constants.
// -------------------------------------------------------------------------------------------------

static const unsigned int PRINT_TELEMETRY_INTERVAL_MILLIS = 500;
static const unsigned int READ_COMPASS_INTERVAL_MILLIS = 1000 / 15; // Default readings rate is 15Hz.
static const unsigned int READ_CONSOLE_INTERVAL_MILLIS = 100;
static const unsigned int MAX_CONSOLE_INPUT_LENGTH = 40;

static const float COMPASS_SMOOTHING_FACTOR = 0.3f;

// -------------------------------------------------------------------------------------------------
// Custom types.
// -------------------------------------------------------------------------------------------------

typedef enum MotorMode {
    FORWARD,
    BACKWARD,
    OFF,
    SHORT_BREAK,
};

typedef struct MotorPins {
    unsigned int pin1;
    unsigned int pin2;
    unsigned int pwm;
};

// -------------------------------------------------------------------------------------------------
// Peripherals and pins.
// -------------------------------------------------------------------------------------------------

static const unsigned int PIN_ROTARY_LEFT = 20;
static const unsigned int PIN_ROTARY_RIGHT = 21;

static const unsigned int PIN_MOTOR_A1 = 4;
static const unsigned int PIN_MOTOR_A2 = 2;
static const unsigned int PIN_MOTOR_A_PWM = 9;

static const unsigned int PIN_MOTOR_B1 = 7;
static const unsigned int PIN_MOTOR_B2 = 8;
static const unsigned int PIN_MOTOR_B_PWM = 10;

static const MotorPins PINS_MOTOR_A = {PIN_MOTOR_A1, PIN_MOTOR_A2, PIN_MOTOR_A_PWM};
static const MotorPins PINS_MOTOR_B = {PIN_MOTOR_B1, PIN_MOTOR_B2, PIN_MOTOR_B_PWM};

Adafruit_HMC5883_Unified compass = Adafruit_HMC5883_Unified();

// -------------------------------------------------------------------------------------------------
// Forward declarations.
// -------------------------------------------------------------------------------------------------

void initializeSerial();
void initializePins();
void initializeCompass();
void startTickers();

void printTelemetry();
void readCompass();
void readConsole();

void incrementRotaryLeft();
void incrementRotaryRight();

void controlMotor(const MotorPins&, const MotorMode, const unsigned int);

void handleConsoleInput(const char[], unsigned int);

// -------------------------------------------------------------------------------------------------
// Tickers.
// Don't forget to call `start` and `update` on them.
// -------------------------------------------------------------------------------------------------

Ticker telemetryTicker(printTelemetry, PRINT_TELEMETRY_INTERVAL_MILLIS);
Ticker readCompassTicker(readCompass, READ_COMPASS_INTERVAL_MILLIS);
Ticker readConsoleTicker(readConsole, READ_CONSOLE_INTERVAL_MILLIS);

// -------------------------------------------------------------------------------------------------
// Current state.
// -------------------------------------------------------------------------------------------------

static bool isTelemetryEnabled = true;

static float compassX = 0.0f;
static float compassY = 0.0f;

static float positionTicksX = 0.0f;
static float positionTicksY = 0.0f;

// -------------------------------------------------------------------------------------------------
// Arduino setup and loop.
// -------------------------------------------------------------------------------------------------

void setup() {
    initializeSerial();

    Serial.println("└[∵]┘ Hi! Press <Enter> to enter the CLI.");

    initializePins();
    initializeCompass();
    startTickers();
}

void loop() {
    telemetryTicker.update();
    readCompassTicker.update();
    readConsoleTicker.update();
}

// -------------------------------------------------------------------------------------------------
// Initializers.
// -------------------------------------------------------------------------------------------------

void initializePins() {
    pinMode(PIN_ROTARY_LEFT, INPUT);
    pinMode(PIN_ROTARY_RIGHT, INPUT);

    attachInterrupt(digitalPinToInterrupt(PIN_ROTARY_LEFT), incrementRotaryLeft, CHANGE);
    attachInterrupt(digitalPinToInterrupt(PIN_ROTARY_RIGHT), incrementRotaryRight, CHANGE);

    pinMode(LED_BUILTIN, OUTPUT);
    
    controlMotor(PINS_MOTOR_A, MotorMode::OFF, 0);
    pinMode(PIN_MOTOR_A1, OUTPUT);
    pinMode(PIN_MOTOR_A2, OUTPUT);
    pinMode(PIN_MOTOR_A_PWM, OUTPUT);

    controlMotor(PINS_MOTOR_B, MotorMode::OFF, 0);
    pinMode(PIN_MOTOR_B1, OUTPUT);
    pinMode(PIN_MOTOR_B2, OUTPUT);
    pinMode(PIN_MOTOR_B_PWM, OUTPUT);
}

void initializeSerial() {
    Serial.begin(9600);
    while (!Serial);
}

void initializeCompass() {
    if (!compass.begin()) {
        Serial.println("┌[∵]┐ Error: no HMC5883 detected.");
        while (1);
    }
}

void startTickers() {
    telemetryTicker.start();
    readCompassTicker.start();
    readConsoleTicker.start();
}

// -------------------------------------------------------------------------------------------------
// Functionality.
// -------------------------------------------------------------------------------------------------

void printTelemetry() {
    if (!isTelemetryEnabled) {
        return;
    }

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
void handleConsoleInput(const char input[], unsigned int length) {
    if (length == 0) {
        isTelemetryEnabled = !isTelemetryEnabled;
        if (isTelemetryEnabled) {
            Serial.println("└[∵]┘ Quitting CLI. Telemetry enabled. Press <Enter> to go back to CLI.");
        } else {
            Serial.println("└[∵]┘ Telemetry disabled. Entering CLI. Press <Enter> to quit.");
        }
    } else if (strncasecmp(input, "start b", length) == 0) {
        controlMotor(PINS_MOTOR_B, MotorMode::FORWARD, 255);
    } else if (strncasecmp(input, "reverse b", length) == 0) {
        controlMotor(PINS_MOTOR_B, MotorMode::BACKWARD, 255);
    } else if (strncasecmp(input, "off b", length) == 0) {
        controlMotor(PINS_MOTOR_B, MotorMode::OFF, 0);
    } else if (strncasecmp(input, "break b", length) == 0) {
        controlMotor(PINS_MOTOR_B, MotorMode::SHORT_BREAK, 255);
    } else {
        Serial.print("┌[∵]┐ I don't understand: `");
        Serial.print(input);
        Serial.println("`");
    }
    if (!isTelemetryEnabled) {
        Serial.print("└[∵]┘ > ");
    }
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

void incrementRotaryLeft() {
    // TODO: wheel rotation direction.
    positionTicksX += 0.5f * compassX;
    positionTicksY += 0.5f * compassY;
}

void incrementRotaryRight() {
    // TODO: wheel rotation direction.
    positionTicksX += 0.5f * compassX;
    positionTicksY += 0.5f * compassY;
}

void controlMotor(const MotorPins &pins, const MotorMode mode, const unsigned int speed) {
    analogWrite(pins.pwm, speed);
    switch (mode) {
        case MotorMode::FORWARD:
            digitalWrite(pins.pin2, LOW);
            digitalWrite(pins.pin1, HIGH);
            break;
        case MotorMode::BACKWARD:
            digitalWrite(pins.pin1, LOW);
            digitalWrite(pins.pin2, HIGH);
            break;
        case MotorMode::OFF:
            digitalWrite(pins.pin1, LOW);
            digitalWrite(pins.pin2, LOW);
            break;
        case MotorMode::SHORT_BREAK:
            digitalWrite(pins.pin1, HIGH);
            digitalWrite(pins.pin2, HIGH);
            break;
    }
}

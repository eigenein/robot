#include <math.h>

#include <Arduino.h>

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <Ticker.h>

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

static const float MILLIS_PER_SEC_F = 1000.0f;

static const size_t MAX_CONSOLE_INPUT_LENGTH = 40;

static const millis_t CONTROL_MOTORS_INTERVAL_MILLIS = 200;

// -------------------------------------------------------------------------------------------------
// Peripherals and pins.
// -------------------------------------------------------------------------------------------------

static const pin_size_t PIN_ROTARY_LEFT = 21;
static const pin_size_t PIN_ROTARY_RIGHT = 20;

static const pin_size_t PIN_MOTOR_RIGHT_1 = 4;
static const pin_size_t PIN_MOTOR_RIGHT_2 = 2;
static const pin_size_t PIN_MOTOR_RIGHT_PWM = 9;

static const pin_size_t PIN_MOTOR_LEFT_1 = 8;
static const pin_size_t PIN_MOTOR_LEFT_2 = 7;
static const pin_size_t PIN_MOTOR_LEFT_PWM = 10;

static const pin_size_t PIN_BUZZER = 17;

static Adafruit_BNO055 orientationSensor = Adafruit_BNO055(-1, 0x29);

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
void controlMotors();

void controlSingleMotor(const pin_size_t, const pin_size_t, const pin_size_t, const int, const bool);
float calculatePID(const float, const float, const float, const float, const float, const float, float&, float&);

void handleConsoleInput(const char[], const size_t);

void onLeftRotaryChange();
void onRightRotaryChange();

// -------------------------------------------------------------------------------------------------
// Tickers.
// Don't forget to call `start` and `update` on them.
// -------------------------------------------------------------------------------------------------

Ticker printTelemetryTicker(printTelemetry, (millis_t)500);
Ticker readConsoleTicker(readConsole, (millis_t)100);
Ticker readOrientationSensorTicker(readOrientationSensor, (millis_t)50);
Ticker controlMotorsTicker(controlMotors, CONTROL_MOTORS_INTERVAL_MILLIS);

// -------------------------------------------------------------------------------------------------
// Current state.
// -------------------------------------------------------------------------------------------------

static bool isTelemetryEnabled = true;

static sensors_event_t orientation, acceleration;
static tick_speed_t targetLeftSpeed = 0, targetRightSpeed = 0;
static tick_speed_t actualLeftSpeed = 0, actualRightSpeed = 0;
static uint8_t leftTicks = 0, rightTicks = 0;
static float leftPIDSignal = 0.0f, rightPIDSignal = 0.0f;

static float motorKp = 0.01f;
static float motorKi = 0.002f;
static float motorKd = 0.5f;

// -------------------------------------------------------------------------------------------------
// Arduino setup and loop.
// -------------------------------------------------------------------------------------------------

void setup() {
    initializeSerial();

    Serial.println("└[∵]┘ Hi! Press <Enter> to enter the CLI.");

    initializePins();
    initializeOrientationSensor();
    startTickers();

    tone(PIN_BUZZER, 440, 50); // Initialization tone.
    Serial.println("└[∵]┐ Initialization delay…");
    delay(100); // TODO: check what kind of delay is actually needed by BNO055.
    Serial.println("└[∵]┘ Initialized.");
    tone(PIN_BUZZER, 880, 50); // Initialization tone.
}

void loop() {
    printTelemetryTicker.update();
    readConsoleTicker.update();
    readOrientationSensorTicker.update();
    controlMotorsTicker.update();
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

    digitalWrite(PIN_MOTOR_LEFT_1, LOW);
    pinMode(PIN_MOTOR_LEFT_1, OUTPUT);
    digitalWrite(PIN_MOTOR_LEFT_2, LOW);
    pinMode(PIN_MOTOR_LEFT_2, OUTPUT);
    digitalWrite(PIN_MOTOR_LEFT_PWM, LOW);
    pinMode(PIN_MOTOR_LEFT_PWM, OUTPUT);

    digitalWrite(PIN_MOTOR_RIGHT_1, LOW);
    pinMode(PIN_MOTOR_RIGHT_1, OUTPUT);
    digitalWrite(PIN_MOTOR_RIGHT_2, LOW);
    pinMode(PIN_MOTOR_RIGHT_2, OUTPUT);
    digitalWrite(PIN_MOTOR_RIGHT_PWM, LOW);
    pinMode(PIN_MOTOR_RIGHT_PWM, OUTPUT);
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
    controlMotorsTicker.start();
}

// -------------------------------------------------------------------------------------------------
// Functionality.
// -------------------------------------------------------------------------------------------------

void printTelemetry() {
    if (!isTelemetryEnabled) {
        return;
    }

    Serial.print(orientation.orientation.x);
    
    Serial.print("° | actual: ");
    Serial.print(actualLeftSpeed);
    Serial.print(" ");
    Serial.print(actualRightSpeed);
    
    Serial.print(" | target: ");
    Serial.print(targetLeftSpeed);
    Serial.print(" ");
    Serial.print(targetRightSpeed);
    
    Serial.print(" | PID: ");
    Serial.print(leftPIDSignal);
    Serial.print(" ");
    Serial.print(rightPIDSignal);

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
    int32_t argument;
    if (length == 0) {
        isTelemetryEnabled = !isTelemetryEnabled;
        if (isTelemetryEnabled) {
            Serial.println("└[∵]┘ Quitting CLI. Telemetry enabled. Press <Enter> to go back to CLI.");
        } else {
            Serial.println("└[∵]┘ Telemetry disabled. Entering CLI. Press <Enter> to quit.");
        }
    } else if (sscanf(input, "l%ld", &argument) == 1) {
        targetLeftSpeed = argument;
    } else if (sscanf(input, "r%ld", &argument) == 1) {
        targetRightSpeed = argument;
    } else if (sscanf(input, "s%ld", &argument) == 1) {
        targetLeftSpeed = targetRightSpeed = argument;
    } else if (sscanf(input, "kp%ld", &argument) == 1) {
        motorKp = argument / 1000.0;
    } else if (sscanf(input, "ki%ld", &argument) == 1) {
        motorKi = argument / 1000.0;
    } else if (sscanf(input, "kd%ld", &argument) == 1) {
        motorKd = argument / 1000.0;
    } else if (!isTelemetryEnabled) {
        Serial.print("┌[∵]┐ I don't understand: `");
        Serial.print(input);
        Serial.println("`");
    }
    if (!isTelemetryEnabled) {
        Serial.print("kp: ");
        Serial.print(motorKp, 3);
        Serial.print(" ki: ");
        Serial.print(motorKi, 3);
        Serial.print(" kd: ");
        Serial.println(motorKd, 3);
        Serial.print("└[∵]┘ > ");
    }
}

void onLeftRotaryChange() {
    leftTicks++;
}

void onRightRotaryChange() {
    rightTicks++;
}

void controlSingleMotor(
    const pin_size_t pin1,
    const pin_size_t pin2,
    const pin_size_t pinPWM,
    const int pidSignal,
    const bool reverse
) {
    analogWrite(pinPWM, constrain(pidSignal, 0, 255));
    if (pidSignal == 0) {
        digitalWrite(pin1, HIGH);
        digitalWrite(pin2, HIGH);
    } else if (!reverse) {
        digitalWrite(pin1, LOW);
        digitalWrite(pin2, HIGH);
    } else {
        digitalWrite(pin2, LOW);
        digitalWrite(pin1, HIGH);
    }
}

void readOrientationSensor() {
    orientationSensor.getEvent(&orientation, Adafruit_BNO055::VECTOR_EULER);
    orientationSensor.getEvent(&acceleration, Adafruit_BNO055::VECTOR_LINEARACCEL);
}

void controlMotors() {
    static float leftIntegral = 0.0f, rightIntegral = 0.0f;
    static float previousLeftError = 0.0f, previousRightError = 0.0f;
    
    actualLeftSpeed = MILLIS_PER_SEC_F * leftTicks / CONTROL_MOTORS_INTERVAL_MILLIS;
    actualRightSpeed = MILLIS_PER_SEC_F * rightTicks / CONTROL_MOTORS_INTERVAL_MILLIS;
    leftTicks = rightTicks = 0;

    leftPIDSignal = calculatePID(
        fabs(targetLeftSpeed),
        actualLeftSpeed,
        motorKp,
        motorKi,
        motorKd,
        CONTROL_MOTORS_INTERVAL_MILLIS,
        leftIntegral,
        previousLeftError
    );
    controlSingleMotor(PIN_MOTOR_LEFT_1, PIN_MOTOR_LEFT_2, PIN_MOTOR_LEFT_PWM, leftPIDSignal, targetLeftSpeed < 0);

    rightPIDSignal = calculatePID(
        fabs(targetRightSpeed),
        actualRightSpeed,
        motorKp,
        motorKi,
        motorKd,
        CONTROL_MOTORS_INTERVAL_MILLIS,
        rightIntegral,
        previousRightError
    );
    controlSingleMotor(PIN_MOTOR_RIGHT_1, PIN_MOTOR_RIGHT_2, PIN_MOTOR_RIGHT_PWM, rightPIDSignal, targetRightSpeed < 0);
}

// https://en.wikipedia.org/wiki/PID_controller#Pseudocode
float calculatePID(
    const float setPoint,
    const float actual,
    const float kp,
    const float ki,
    const float kd,
    const float elapsed,
    float& integral,
    float& previousError
) {
    const float error = setPoint - actual;
    const float proportional = error;
    integral += error * elapsed;
    const float derivative = (error - previousError) / elapsed;
    previousError = error;
    return kp * proportional + ki * integral + kd * derivative;
}

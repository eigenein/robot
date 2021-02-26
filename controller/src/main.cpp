#include <Arduino.h>

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>
#include <Ticker.h>

Adafruit_HMC5883_Unified compass = Adafruit_HMC5883_Unified();

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);

    Serial.begin(9600);
    while (!Serial);

    if (!compass.begin()) {
        Serial.println("Error: no HMC5883 detected.");
        while(1);
    }
}

void loop() {
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
    digitalWrite(LED_BUILTIN, HIGH);

    sensors_event_t event; 
    compass.getEvent(&event);

    float magnitude = sqrtf(event.magnetic.x * event.magnetic.x + event.magnetic.y * event.magnetic.y);
    Serial.print(event.magnetic.x / magnitude); Serial.print(" | ");
    Serial.print(event.magnetic.y / magnitude); Serial.print(" | ");
    Serial.print(atan2(event.magnetic.y, event.magnetic.x) * 180.0f / M_PI); Serial.println("");
}

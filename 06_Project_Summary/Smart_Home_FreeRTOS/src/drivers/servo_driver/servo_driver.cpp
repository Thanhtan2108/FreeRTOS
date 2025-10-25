#include "servo_driver.h"

ServoDriver::ServoDriver() : initialized(false), currentAngle(90) {}

bool ServoDriver::begin() {
    // Allow allocation of all timers
    ESP32PWM::allocateTimer(0);
    ESP32PWM::allocateTimer(1);
    ESP32PWM::allocateTimer(2);
    ESP32PWM::allocateTimer(3);
    
    servo.setPeriodHertz(50);    // Standard 50hz servo
    if (servo.attach(SERVO_PIN, 500, 2400)) { // Attach servo to pin
        Serial.println("Servo: Initialized successfully");
        initialized = true;
        setAngle(90); // Start at middle position
    } else {
        Serial.println("Servo: Failed to attach to pin");
        initialized = false;
    }
    
    return initialized;
}

void ServoDriver::setAngle(uint8_t angle) {
    if (!initialized) {
        Serial.println("Servo: Not initialized");
        return;
    }
    
    if (angle > 180) angle = 180;
    
    servo.write(angle);
    currentAngle = angle;
    Serial.printf("Servo: Set to %d degrees\n", angle);
}

uint8_t ServoDriver::getCurrentAngle() {
    return currentAngle;
}

bool ServoDriver::isConnected() {
    return initialized;
}

#ifndef SERVO_DRIVER_H
#define SERVO_DRIVER_H

#include <ESP32Servo.h>
#include "../../config/pin_config.h"

class ServoDriver {
public:
    ServoDriver();
    bool begin();
    void setAngle(uint8_t angle);
    uint8_t getCurrentAngle();
    bool isConnected();
    
private:
    Servo servo;
    bool initialized;
    uint8_t currentAngle;
};

#endif

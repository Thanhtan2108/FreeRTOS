#ifndef DATA_TYPES_H
#define DATA_TYPES_H

#include <stdint.h>

typedef struct {
    float temperature;
    float humidity; 
    float lightLevel;
    float airQuality;
    uint32_t timestamp;
} SensorData_t;

typedef struct {
    uint8_t deviceType;  // 0: relay, 1: servo, 2: fan
    uint8_t state;       // 0: off, 1: on
    uint8_t value;       // angle or speed
} ControlCommand_t;

typedef enum {
    DEVICE_RELAY = 0,
    DEVICE_SERVO = 1,
    DEVICE_FAN = 2
} DeviceType_t;

#endif

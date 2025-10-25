#ifndef DS18B20_DRIVER_H
#define DS18B20_DRIVER_H

#include <OneWire.h>
#include <DallasTemperature.h>
#include "../../config/pin_config.h"  // Đã cập nhật

class DS18B20Driver {
public:
    DS18B20Driver();
    bool begin();
    float readTemperature();
    bool isConnected();
    int getDeviceCount() { return deviceCount; }
    
private:
    OneWire oneWire;
    DallasTemperature sensors;
    bool initialized;
    int deviceCount;
};

#endif

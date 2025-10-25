#include "ds18b20_driver.h"

DS18B20Driver::DS18B20Driver() 
    : oneWire(DS18B20_PIN), sensors(&oneWire), initialized(false) {}

bool DS18B20Driver::begin() {
    sensors.begin();
    deviceCount = sensors.getDeviceCount();
    Serial.printf("DS18B20: Found %d devices\n", deviceCount);
    initialized = (deviceCount > 0);
    return initialized;
}

float DS18B20Driver::readTemperature() {
    if (!initialized) {
        Serial.println("DS18B20: Not initialized");
        return -127.0f;
    }
    
    sensors.requestTemperatures();
    float temp = sensors.getTempCByIndex(0);
    
    if (temp == DEVICE_DISCONNECTED_C) {
        Serial.println("DS18B20: Error reading temperature");
        return -127.0f;
    }
    
    return temp;
}

bool DS18B20Driver::isConnected() {
    return initialized;
}

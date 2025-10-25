#ifndef OLED_DRIVER_H
#define OLED_DRIVER_H

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "../../config/pin_config.h"  // Đã cập nhật

class OLEDDriver {
public:
    OLEDDriver();
    bool begin();
    void clear();
    void displaySensorData(float temp, float humidity, float light, float airQuality);
    void displaySystemStatus(const char* status);
    void displayTestScreen();
    
private:
    Adafruit_SSD1306 display;
    bool initialized;
};

#endif

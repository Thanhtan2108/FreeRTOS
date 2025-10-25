#include "bh1750_driver.h"

BH1750Driver::BH1750Driver() : initialized(false) {}

bool BH1750Driver::begin() {
    Wire.begin(I2C_SDA, I2C_SCL);
    
    if (lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
        Serial.println("BH1750: Initialized successfully");
        initialized = true;
    } else {
        Serial.println("BH1750: Initialization failed");
        initialized = false;
    }
    return initialized;
}

float BH1750Driver::readLightLevel() {
    if (!initialized) {
        Serial.println("BH1750: Not initialized");
        return -1.0f;
    }
    
    float lux = lightMeter.readLightLevel();
    if (lux < 0) {
        Serial.println("BH1750: Error reading light level");
        return -1.0f;
    }
    
    return lux;
}

bool BH1750Driver::isConnected() {
    return initialized;
}

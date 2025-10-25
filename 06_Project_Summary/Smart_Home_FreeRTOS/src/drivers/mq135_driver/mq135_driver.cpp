#include "mq135_driver.h"
#include <Arduino.h>

// Calibration constants for MQ135
#define RLOAD 10.0
#define RZERO 76.63
#define PARA 116.6020682
#define PARB 2.769034857

MQ135Driver::MQ135Driver() : initialized(false) {}

bool MQ135Driver::begin() {
    pinMode(MQ135_PIN, INPUT);
    initialized = true;
    Serial.println("MQ135: Initialized (warming up... need 24-48 hours for accurate readings)");
    return initialized;
}

float MQ135Driver::getResistance() {
    int val = analogRead(MQ135_PIN);
    if (val == 0) {
        // Avoid division by zero
        return -1.0f;
    }
    return ((4095.0 / (float)val) - 1.0) * RLOAD;
}

float MQ135Driver::getCorrectedResistance(float temperature, float humidity) {
    // Basic environmental correction
    float resistance = getResistance();
    if (resistance < 0) return -1.0f;
    
    // Simplified correction - in real application, use more complex formula
    return resistance * (1.0 - (humidity / 100.0 * 0.1)) * (1.0 - ((temperature - 20.0) / 100.0 * 0.1));
}

float MQ135Driver::readAirQuality() {
    if (!initialized) return -1.0f;
    
    float resistance = getResistance();
    if (resistance < 0) return -1.0f;
    
    // Convert to PPM (simplified calculation)
    float ppm = PARA * pow((resistance / RZERO), -PARB);
    
    return ppm;
}

float MQ135Driver::getCorrectedPPM(float temperature, float humidity) {
    if (!initialized) return -1.0f;
    
    float correctedResistance = getCorrectedResistance(temperature, humidity);
    if (correctedResistance < 0) return -1.0f;
    
    float ppm = PARA * pow((correctedResistance / RZERO), -PARB);
    return ppm;
}

bool MQ135Driver::isConnected() {
    return initialized;
}

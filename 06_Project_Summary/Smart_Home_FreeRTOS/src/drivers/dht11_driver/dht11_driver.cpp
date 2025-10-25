#include "dht11_driver.h"

DHT11Driver::DHT11Driver() 
    : dht(DHT11_PIN, DHT11), initialized(false) {}

bool DHT11Driver::begin() {
    dht.begin();
    initialized = true;
    
    // Test read to verify connection
    float test = dht.readTemperature();
    if (isnan(test)) {
        Serial.println("DHT11: No sensor found");
        initialized = false;
    } else {
        Serial.println("DHT11: Initialized successfully");
    }
    
    return initialized;
}

float DHT11Driver::readTemperature() {
    if (!initialized) return -127.0f;
    
    float temp = dht.readTemperature();
    if (isnan(temp)) {
        Serial.println("DHT11: Error reading temperature");
        return -127.0f;
    }
    return temp;
}

float DHT11Driver::readHumidity() {
    if (!initialized) return -1.0f;
    
    float humidity = dht.readHumidity();
    if (isnan(humidity)) {
        Serial.println("DHT11: Error reading humidity");
        return -1.0f;
    }
    return humidity;
}

bool DHT11Driver::isConnected() {
    return initialized;
}

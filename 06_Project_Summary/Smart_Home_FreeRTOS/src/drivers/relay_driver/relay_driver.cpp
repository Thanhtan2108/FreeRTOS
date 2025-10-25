#include "relay_driver.h"
#include <Arduino.h>

RelayDriver::RelayDriver() : initialized(false), currentState(false) {}

bool RelayDriver::begin() {
    pinMode(RELAY_PIN, OUTPUT);
    digitalWrite(RELAY_PIN, LOW); // Start with relay OFF
    initialized = true;
    currentState = false;
    
    Serial.println("Relay: Initialized successfully (starting OFF)");
    return initialized;
}

void RelayDriver::setState(bool state) {
    if (!initialized) {
        Serial.println("Relay: Not initialized");
        return;
    }
    
    digitalWrite(RELAY_PIN, state ? HIGH : LOW);
    currentState = state;
    
    Serial.printf("Relay: Set to %s\n", state ? "ON" : "OFF");
}

void RelayDriver::toggle() {
    setState(!currentState);
}

bool RelayDriver::getState() {
    return currentState;
}

bool RelayDriver::isConnected() {
    return initialized;
}

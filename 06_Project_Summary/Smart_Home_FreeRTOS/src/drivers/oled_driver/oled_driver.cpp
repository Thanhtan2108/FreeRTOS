#include "oled_driver.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

OLEDDriver::OLEDDriver() 
    : display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1), initialized(false) {}

bool OLEDDriver::begin() {
    Wire.begin(I2C_SDA, I2C_SCL);
    
    if(!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
        Serial.println("OLED: Allocation failed");
        return false;
    }
    
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);
    initialized = true;
    
    Serial.println("OLED: Initialized successfully");
    return true;
}

void OLEDDriver::clear() {
    if (!initialized) return;
    display.clearDisplay();
}

void OLEDDriver::displaySensorData(float temp, float humidity, float light, float airQuality) {
    if (!initialized) return;
    
    display.clearDisplay();
    display.setCursor(0, 0);
    
    display.println("=== SMART HOME ===");
    display.printf("Temp: %.1fC\n", temp);
    display.printf("Humidity: %.1f%%\n", humidity);
    display.printf("Light: %.0flux\n", light);
    display.printf("Air Q: %.0f\n", airQuality);
    display.println("------------------");
    
    display.display();
}

void OLEDDriver::displaySystemStatus(const char* status) {
    if (!initialized) return;
    
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("=== SYSTEM STATUS ===");
    display.println(status);
    display.display();
}

void OLEDDriver::displayTestScreen() {
    if (!initialized) return;
    
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("OLED TEST SCREEN");
    display.println("Line 1: OK");
    display.println("Line 2: OK"); 
    display.println("Line 3: OK");
    display.println("Line 4: OK");
    display.display();
}

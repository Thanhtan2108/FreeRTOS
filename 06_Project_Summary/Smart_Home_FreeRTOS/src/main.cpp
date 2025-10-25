#include <Arduino.h>

// Include tất cả drivers
#include "drivers/ds18b20_driver/ds18b20_driver.h"
#include "drivers/dht11_driver/dht11_driver.h" 
#include "drivers/oled_driver/oled_driver.h"
#include "drivers/bh1750_driver/bh1750_driver.h"
#include "drivers/mq135_driver/mq135_driver.h"
#include "drivers/servo_driver/servo_driver.h"
#include "drivers/relay_driver/relay_driver.h"
#include "config/pin_config.h"

// Global driver instances
DS18B20Driver tempSensor;
DHT11Driver dhtSensor;
OLEDDriver oled;
BH1750Driver lightSensor;
MQ135Driver airQualitySensor;
ServoDriver servo;
RelayDriver relay;

// Function test for module


void setup() {
    Serial.begin(115200);
    delay(2000); // Chờ Serial ready
    
    Serial.println("\n🤖 SMART HOME EMBEDDED SYSTEM");
    Serial.println("📅 Phase: Week 1 - Unit Testing");
    Serial.println("🎯 Goal: Verify all hardware modules");
    
    // Chạy function module test
    
}

void loop() {
    // 🛑 KHÔNG làm gì cả - chỉ unit test một lần
    // Em có thể thêm LED blink để biết board vẫn sống
    static unsigned long lastBlink = 0;
    if (millis() - lastBlink > 1000) {
        digitalWrite(2, !digitalRead(2)); // LED built-in
        lastBlink = millis();
    }
}

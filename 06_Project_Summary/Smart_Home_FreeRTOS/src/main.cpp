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

void testDS18B20() {
    Serial.println("\n🧪 === TESTING DS18B20 ===");
    
    if (!tempSensor.begin()) {
        Serial.println("❌ DS18B20: INIT FAILED");
        return;
    }
    
    Serial.println("✅ DS18B20: INIT SUCCESS");
    Serial.printf("📡 Found %d devices\n", tempSensor.getDeviceCount());
    
    for(int i = 0; i < 3; i++) {
        float temp = tempSensor.readTemperature();
        if (temp != -127.0f) {
            Serial.printf("✅ Reading %d: %.2f °C\n", i+1, temp);
        } else {
            Serial.printf("❌ Reading %d: FAILED\n", i+1);
        }
        delay(1000);
    }
}

void testDHT11() {
    Serial.println("\n🧪 === TESTING DHT11 ===");
    
    if (!dhtSensor.begin()) {
        Serial.println("❌ DHT11: INIT FAILED");
        return;
    }
    
    Serial.println("✅ DHT11: INIT SUCCESS");
    
    for(int i = 0; i < 3; i++) {
        float temp = dhtSensor.readTemperature();
        float humidity = dhtSensor.readHumidity();
        
        if (temp != -127.0f && humidity != -1.0f) {
            Serial.printf("✅ Reading %d: %.2f °C, %.2f%% humidity\n", i+1, temp, humidity);
        } else {
            Serial.printf("❌ Reading %d: FAILED\n", i+1);
        }
        delay(2000);
    }
}

void testOLED() {
    Serial.println("\n🧪 === TESTING OLED ===");
    
    if (!oled.begin()) {
        Serial.println("❌ OLED: INIT FAILED");
        return;
    }
    
    Serial.println("✅ OLED: INIT SUCCESS");
    
    // Test sequence
    oled.displayTestScreen();
    Serial.println("📟 Test Screen Displayed");
    delay(2000);
    
    oled.displaySystemStatus("Sensor Testing...");
    Serial.println("📟 Status Screen Displayed");
    delay(2000);
}

void testBH1750() {
    Serial.println("\n🧪 === TESTING BH1750 ===");
    
    if (!lightSensor.begin()) {
        Serial.println("❌ BH1750: INIT FAILED");
        return;
    }
    
    Serial.println("✅ BH1750: INIT SUCCESS");
    
    for(int i = 0; i < 3; i++) {
        float light = lightSensor.readLightLevel();
        if (light >= 0) {
            Serial.printf("✅ Reading %d: %.2f lux\n", i+1, light);
        } else {
            Serial.printf("❌ Reading %d: FAILED\n", i+1);
        }
        delay(1000);
    }
}

void testMQ135() {
    Serial.println("\n🧪 === TESTING MQ135 ===");
    
    if (!airQualitySensor.begin()) {
        Serial.println("❌ MQ135: INIT FAILED");
        return;
    }
    
    Serial.println("✅ MQ135: INIT SUCCESS");
    Serial.println("⚠️  Note: MQ135 needs 24-48 hours warm-up for accurate readings");
    
    for(int i = 0; i < 3; i++) {
        float airQuality = airQualitySensor.readAirQuality();
        if (airQuality >= 0) {
            Serial.printf("📊 Reading %d: %.0f ppm\n", i+1, airQuality);
        } else {
            Serial.printf("❌ Reading %d: FAILED\n", i+1);
        }
        delay(1000);
    }
}

void testServo() {
    Serial.println("\n🧪 === TESTING SERVO ===");
    
    if (!servo.begin()) {
        Serial.println("❌ Servo: INIT FAILED");
        return;
    }
    
    Serial.println("✅ Servo: INIT SUCCESS");
    
    // Test sequence với kiểm tra lỗi
    int testAngles[] = {0, 90, 180, 90};
    for(int i = 0; i < 4; i++) {
        servo.setAngle(testAngles[i]);
        Serial.printf("🔄 Moved to %d degrees\n", testAngles[i]);
        delay(1500);
    }
}

void testRelay() {
    Serial.println("\n🧪 === TESTING RELAY ===");
    
    if (!relay.begin()) {
        Serial.println("❌ Relay: INIT FAILED");
        return;
    }
    
    Serial.println("✅ Relay: INIT SUCCESS");
    
    // Test sequence
    Serial.println("🔌 Turning relay ON");
    relay.setState(true);
    delay(1000);
    
    Serial.println("🔌 Turning relay OFF");
    relay.setState(false);
    delay(1000);
    
    Serial.println("🔌 Toggling relay (should be ON)");
    relay.toggle();
    delay(1000);
    
    Serial.println("🔌 Toggling relay (should be OFF)");
    relay.toggle();
    delay(1000);
}

void runAllTests() {
    Serial.println("\n" + String(50, '='));
    Serial.println("🚀 STARTING COMPREHENSIVE UNIT TESTS");
    Serial.println(String(50, '='));
    
    // Test từng module theo thứ tự
    testOLED();
    testDS18B20(); 
    testDHT11();
    testBH1750();
    testMQ135();
    testServo();
    testRelay();
    
    Serial.println("\n" + String(50, '='));
    Serial.println("🎉 ALL UNIT TESTS COMPLETED!");
    Serial.println("📋 Summary: Check above for ✅ SUCCESS / ❌ FAILED");
    Serial.println("💡 Next: Fix any failed modules before integration");
    Serial.println(String(50, '='));
    
    // Hiển thị kết thúc trên OLED
    oled.displaySystemStatus("Unit Tests Complete!");
}

void setup() {
    Serial.begin(115200);
    delay(2000); // Chờ Serial ready
    
    Serial.println("\n🤖 SMART HOME EMBEDDED SYSTEM");
    Serial.println("📅 Phase: Week 1 - Unit Testing");
    Serial.println("🎯 Goal: Verify all hardware modules");
    
    // Chạy tất cả tests
    runAllTests();
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

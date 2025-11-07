# Tuần 1

## Thiết lập project PlatformIO + FreeRTOS

### CẤU TRÚC THƯ MỤC

```text
Smart_Home_FreeRTOS/
├── include/                 # Common headers
├── src/
│   ├── main.cpp            # Application entry point
│   ├── tasks/              # FreeRTOS task implementations
│   ├── drivers/            # Hardware drivers
│   ├── services/           # Business logic services
│   ├── utils/              # Utilities and helpers
│   └── config/             # Configuration files
├── lib/                    # External libraries
└── platformio.ini
```

### THIẾT LẬP PROJECT PLATFORMIO

- Khi tạo thư mục trên platformIO tại nơi muốn lưu project, 1 file platformio.ini sẽ được khởi tạo chạy đầu tiên, đây là nơi cấu hình cho project của mình

- Thiết lập cấu hình cho project trong file platformio.ini này:

```ini
[env:esp32doit-devkit-v1]
platform = espressif32
board = esp32doit-devkit-v1
framework = arduino

; Cấu hình cho tốc độ baud để xem log trên 
monitor_speed = 115200

; vào libraries trên PlatformIO để tải các thư viện cho các cảm biến, giao thức hỗ trợ
lib_deps = 
    ; Thư viện dùng để dùng chung các thư viện của Adafruit
    adafruit/Adafruit Unified Sensor@^1.1.15
    ; thư viện dùng cho OLED 0.96 inch của Adafruit
    adafruit/Adafruit GFX Library@^1.12.3
    adafruit/Adafruit SSD1306@^2.5.15
    ; thư viện dùng cho OLED 0.96 inch nhưng nhẹ hơn Adafruit
    olikraus/U8g2@^2.36.15
    ; thư viện dùng cho DHT của Adafruit
    adafruit/DHT sensor library@^1.4.6
    ; thư viện dùng cho DS18B20
    milesburton/DallasTemperature@^4.0.5
    ; thư viện dùng cho BH1750
    claws/BH1750@^1.3.0
    ; thư viện dùng kết nối Firebase cho ESP32
    mobizt/Firebase ESP32 Client@^4.4.17
    ; thư viện dùng cho Servo trên ESP32
    madhephaestus/ESP32Servo@^3.0.9

; đọc thêm giải thích trong Explain1.md (đường dẫn bên dưới) để hiểu các thông số của các lệnh bên dưới
build_flags = 
    -D CORE_DEBUG_LEVEL=1
    -Wl,-Map=output.map

monitor_filters = 
    log2file
    time
```

**[Xem giải thích trong Explain1.md tại đây](./Explain1.md)**

## Code driver cho từng cảm biến

### 1. Module Config

- src/config/pin_config.h

### 2. Module DS18B20 Driver

Wiring DS18B20:

```text
VCC  → 3.3V
GND  → GND  
DATA → GPIO 4 (cần điện trở 4.7K kéo lên VCC)
```

- src/drivers/ds18b20_driver/ds18b20_driver.h

- src/drivers/ds18b20_driver/ds18b20_driver.cpp

### 3. Module DHT11 Driver

Wiring DHT11:

```text
VCC  → 3.3V
GND  → GND
DATA → GPIO 5
```

- src/drivers/dht11_driver/dht11_driver.h

- src/drivers/dht11_driver/dht11_driver.cpp

### 4. Module OLED Display

Wiring OLED:

```text
VCC → 3.3V
GND → GND
SCL → GPIO 22
SDA → GPIO 21
```

- src/drivers/oled_driver/oled_driver.h

- src/drivers/oled_driver/oled_driver.cpp

### 5. Module BH1750 Driver

Wiring BH1750

```text
VCC → 3.3V
GND → GND
SCL → GPIO 22
SDA → GPIO 21
```

- src/drivers/bh1750_driver/bh1750_driver.h

- src/drivers/bh1750_driver/bh1750_driver.cpp

### 6. Module MQ135 Driver

Wiring MQ135

```text
VCC → 5V (hoặc 3.3V tùy module)
GND → GND
AO  → GPIO 34 (chân analog)
DO  → Không kết nối (nếu có)
```

- src/drivers/mq135_driver/mq135_driver.h

- src/drivers/mq135_driver/mq135_driver.cpp

### 7. Module Servo Driver

Wiring Servo

```text
Dây cam (tín hiệu) → GPIO 18
Dây đỏ (VCC) → 5V (KHÔNG dùng 3.3V - servo cần dòng lớn hơn)
Dây nâu (GND) → GND
```

- src/drivers/servo_driver/servo_driver.h

- src/drivers/servo_driver/servo_driver.cpp

### 8. Module Relay Driver

Wiring Relay

```text
VCC  → 5V
GND  → GND
IN   → GPIO 23
```

- src/drivers/relay_driver/relay_driver.h

- src/drivers/relay_driver/relay_driver.cpp

### 9. Module Data Structures

- include/data_types.h

## Test từng sensor riêng lẻ

- src/main.cpp (Test Version):

```cpp
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
```

## 🎯 CÔNG VIỆC CỤ THỂ TỪNG NGÀY TRONG TUẦN 1

### 📅 Ngày 1-2

- Tạo project PlatformIO với cấu trúc thư mục như trên

- Triển khai 3 module driver đầu tiên (DS18B20, DHT11, OLED)

- Test từng module độc lập

### 📅 Ngày 3-4

- Triển khai các module driver còn lại (BH1750, MQ135, Servo, Relay)

- Viết test code cho từng module

- Debug kết nối phần cứng

### 📅 Ngày 5-7

- Hoàn thiện tất cả drivers

- Viết comprehensive test cho tất cả modules

- Document code và wiring diagram

## 🧪 HƯỚNG DẪN TEST CHI TIẾT TỪNG MODULE

Dưới đây là hướng dẫn test từng module một cách hệ thống. Em nên test theo thứ tự này để dễ debug.

### 📋 QUY TRÌNH TEST TỔNG QUAN

#### Bước 1: Chuẩn bị phần cứng

- ESP32 DEVKIT V1

- Breadboard và dây kết nối

- Module cần test

- Nguồn 5V cho servo (quan trọng!)

#### Bước 2: Kết nối từng module

- Test module nào thì kết nối module đó

#### Bước 3: Upload code và quan sát Serial Monitor

### 🎯 HƯỚNG DẪN TEST TỪNG MODULE

#### 1. 🖥️ TEST OLED DISPLAY

```cpp
// Kết nối:
// VCC → 3.3V
// GND → GND  
// SCL → GPIO 22
// SDA → GPIO 21

void testOLED() {
    Serial.println("\n🧪 === TESTING OLED ===");
    
    if (!oled.begin()) {
        Serial.println("❌ OLED: INIT FAILED - Kiểm tra dây I2C");
        return;
    }
    
    Serial.println("✅ OLED: INIT SUCCESS");
    
    // Test 1: Màn hình test
    oled.displayTestScreen();
    Serial.println("📟 Kiểm tra: Màn hình có hiển thị 4 dòng chữ?");
    delay(3000);
    
    // Test 2: Status screen
    oled.displaySystemStatus("OLED Test OK!");
    Serial.println("📟 Kiểm tra: Màn hình có hiển thị status?");
    delay(2000);
    
    // Test 3: Sensor data
    oled.displaySensorData(25.5, 60.0, 350, 120);
    Serial.println("📟 Kiểm tra: Màn hình có hiển thị sensor data?");
    delay(3000);
}
```

Kết quả mong đợi:

- ✅ Màn hình sáng lên

- ✅ Hiển thị rõ ràng các dòng chữ

- ✅ Không bị nháy hay mất hình

Lỗi thường gặp:

- ❌ Màn hình không sáng → Kiểm tra nguồn 3.3V

- ❌ Không hiển thị chữ → Kiểm tra SDA/SCL (GPIO 21, 22)

- ❌ Chữ mờ → Điều chỉnh độ tương phản (nếu có)

#### 2. 🌡️ TEST DS18B20 TEMPERATURE SENSOR

```cpp
// Kết nối:
// VCC → 3.3V
// GND → GND
// DATA → GPIO 4 (cần điện trở 4.7K kéo lên VCC)

void testDS18B20() {
    Serial.println("\n🧪 === TESTING DS18B20 ===");
    
    if (!tempSensor.begin()) {
        Serial.println("❌ DS18B20: INIT FAILED");
        Serial.println("🔍 Kiểm tra:");
        Serial.println("   - Điện trở 4.7K giữa VCC và DATA");
        Serial.println("   - DS18B20 có 3 chân: VCC, GND, DATA");
        Serial.println("   - Chân DATA nối GPIO 4");
        return;
    }
    
    Serial.println("✅ DS18B20: INIT SUCCESS");
    Serial.printf("📡 Found %d devices\n", tempSensor.getDeviceCount());
    
    for(int i = 0; i < 5; i++) {
        float temp = tempSensor.readTemperature();
        if (temp != -127.0f) {
            Serial.printf("✅ Reading %d: %.2f °C\n", i+1, temp);
            
            // Kiểm tra giá trị hợp lý
            if (temp < -50 || temp > 125) {
                Serial.println("⚠️  Cảnh báo: Nhiệt độ ngoài phạm vi hợp lý");
            }
        } else {
            Serial.printf("❌ Reading %d: FAILED - Kiểm tra kết nối\n", i+1);
        }
        delay(1000);
    }
}
```

Kết quả mong đợi:

- ✅ Tìm thấy ít nhất 1 device

- ✅ Nhiệt độ trong khoảng 15-35°C (nhiệt độ phòng)

- ✅ Giá trị ổn định, không nhảy loạn

Lỗi thường gặp:

- ❌ "Found 0 devices" → Quên điện trở pull-up 4.7K

- ❌ Luôn trả về -127°C → Chân DATA nối sai

- ❌ Giá trị không ổn định → Nguồn không ổn định

#### 3. 💧 TEST DHT11 TEMPERATURE & HUMIDITY

```cpp
// Kết nối:
// VCC → 3.3V  
// GND → GND
// DATA → GPIO 5

void testDHT11() {
    Serial.println("\n🧪 === TESTING DHT11 ===");
    
    if (!dhtSensor.begin()) {
        Serial.println("❌ DHT11: INIT FAILED");
        Serial.println("🔍 Kiểm tra:");
        Serial.println("   - DHT11 có 3-4 chân (bỏ chân NC nếu có 4 chân)");
        Serial.println("   - Chân DATA nối GPIO 5");
        Serial.println("   - Khoảng cách dây ngắn (<20cm)");
        return;
    }
    
    Serial.println("✅ DHT11: INIT SUCCESS");
    
    for(int i = 0; i < 5; i++) {
        float temp = dhtSensor.readTemperature();
        float humidity = dhtSensor.readHumidity();
        
        if (temp != -127.0f && humidity != -1.0f) {
            Serial.printf("✅ Reading %d: %.2f °C, %.2f%% humidity\n", i+1, temp, humidity);
            
            // Kiểm tra giá trị hợp lý
            if (temp < 0 || temp > 50) {
                Serial.println("⚠️  Cảnh báo: Nhiệt độ DHT11 ngoài phạm vi hợp lý");
            }
            if (humidity < 20 || humidity > 90) {
                Serial.println("⚠️  Cảnh báo: Độ ẩm ngoài phạm vi hợp lý");
            }
        } else {
            Serial.printf("❌ Reading %d: FAILED - Kiểm tra kết nối\n", i+1);
        }
        delay(2000); // DHT11 cần 2s giữa các lần đọc
    }
}
```

Kết quả mong đợi:

- ✅ Nhiệt độ: 15-35°C

- ✅ Độ ẩm: 30-80%

- ✅ Giá trị ổn định sau vài lần đọc

Lỗi thường gặp:

- ❌ "NaN" hoặc -127°C → Kết nối lỏng, dây quá dài

- ❌ Giá trị nhảy loạn → Nguồn không ổn định

- ❌ Không đọc được → Chờ đủ 2s giữa các lần đọc

#### 4. 💡 TEST BH1750 LIGHT SENSOR

```cpp
// Kết nối:
// VCC → 3.3V
// GND → GND  
// SCL → GPIO 22
// SDA → GPIO 21

void testBH1750() {
    Serial.println("\n🧪 === TESTING BH1750 ===");
    
    if (!lightSensor.begin()) {
        Serial.println("❌ BH1750: INIT FAILED");
        Serial.println("🔍 Kiểm tra:");
        Serial.println("   - Đã kết nối I2C đúng (cùng bus với OLED)");
        Serial.println("   - Địa chỉ I2C không conflict (OLED: 0x3C, BH1750: 0x23)");
        return;
    }
    
    Serial.println("✅ BH1750: INIT SUCCESS");
    
    for(int i = 0; i < 5; i++) {
        float light = lightSensor.readLightLevel();
        if (light >= 0) {
            Serial.printf("✅ Reading %d: %.2f lux\n", i+1, light);
            
            // Kiểm tra giá trị theo điều kiện ánh sáng
            if (light < 10) {
                Serial.println("💡 Môi trường: Rất tối");
            } else if (light < 100) {
                Serial.println("💡 Môi trường: Tối");
            } else if (light < 1000) {
                Serial.println("💡 Môi trường: Sáng vừa");
            } else {
                Serial.println("💡 Môi trường: Rất sáng");
            }
        } else {
            Serial.printf("❌ Reading %d: FAILED\n", i+1);
        }
        delay(1000);
    }
}
```

Kết quả mong đợi:

- ✅ Giá trị lux >= 0

- ✅ Thay đổi khi che/chiếu sáng cảm biến

- ✅ Trong nhà: 100-500 lux, ngoài trời: 1000+ lux

Lỗi thường gặp:

- ❌ Không tìm thấy device → Kiểm tra địa chỉ I2C

- ❌ Giá trị âm → Lỗi đọc I2C

- ❌ Không thay đổi → Cảm biến bị hỏng

#### 5. 🌫️ TEST MQ135 AIR QUALITY SENSOR

```cpp
// Kết nối:
// VCC → 5V (hoặc 3.3V tùy module)
// GND → GND
// AO → GPIO 34 (chân analog)

void testMQ135() {
    Serial.println("\n🧪 === TESTING MQ135 (CALIBRATED) ===");
    
    if (!airQualitySensor.begin()) {
        Serial.println("❌ MQ135: INIT FAILED");
        return;
    }
    
    Serial.println("✅ MQ135: INIT SUCCESS");
    
    Serial.println("\n📊 Giá trị tham khảo:");
    Serial.println("   - Không khí sạch: 10-600 ppm");
    Serial.println("   - Không khí bình thường: 610-1000 ppm");
    Serial.println("   - Ô nhiễm: 1000+ ppm");
    
    Serial.println("📊 Testing with simplified calibration:");
    for(int i = 0; i < 5; i++) {
        int raw = airQualitySensor.getRawValue();
        float ppm = airQualitySensor.getSimpleAirQuality();
        
        Serial.printf("📈 Reading %d: RAW=%d, PPM=%.0f\n", i+1, raw, ppm);
        
        // Đánh giá đơn giản
        if (ppm < 600) {
            Serial.println("🌬️  Chất lượng: Tốt");
        } else if (ppm < 1000) {
            Serial.println("🌬️  Chất lượng: Trung bình");
        } else {
            Serial.println("🌬️  Chất lượng: Ô nhiễm - Cần cải thiện");
        }
        delay(2000);
    }
}
```

Kết quả mong đợi:

- ✅ Giá trị ppm >= 0

- ✅ Thay đổi khi thổi hơi vào cảm biến

- ✅ Ổn định dần sau vài phút

Lỗi thường gặp:

- ❌ Giá trị luôn bằng 0 → Chân AO nối sai

- ❌ Giá trị quá cao → Cần warm-up lâu hơn

- ❌ Không thay đổi → Cảm biến bị hỏng

#### 6. 🎯 TEST SERVO MOTOR

```cpp
// Kết nối QUAN TRỌNG:
// Dây cam (tín hiệu) → GPIO 25
// Dây đỏ (VCC) → 5V (KHÔNG dùng 3.3V)
// Dây nâu (GND) → GND

void testServo() {
    Serial.println("\n🧪 === TESTING SERVO (DEBUG) ===");
    
    Serial.println("🔧 Step 1: Initializing servo...");
    if (!servo.begin()) {
        Serial.println("❌ Servo: INIT FAILED");
        Serial.println("🔍 TROUBLESHOOTING:");
        Serial.println("   1. Kiểm tra nguồn 5V từ buck converter");
        Serial.println("   2. Kiểm tra dây GND chung giữa ESP32 và servo");
        Serial.println("   3. Kiểm tra chân tín hiệu GPIO 25");
        Serial.println("   4. Thử servo khác (nếu có)");
        return;
    }
    
    Serial.println("✅ Servo: INIT SUCCESS");
    
    // Test từng góc với thông báo rõ ràng
    int testAngles[] = {0, 45, 90, 135, 180};
    
    for(int i = 0; i < 5; i++) {
        Serial.printf("\n🎯 Test %d: Moving to %d degrees\n", i+1, testAngles[i]);
        servo.setAngle(testAngles[i]);
        
        Serial.println("👀 PLEASE CHECK:");
        Serial.println("   - Servo có quay không?");
        Serial.println("   - Có tiếng động gì không?");
        Serial.println("   - Servo có giữ vị trí không?");
        delay(3000);
    }
    
    // Trở về vị trí an toàn
    servo.setAngle(90);
    Serial.println("\n✅ Servo test sequence completed");
}
```

Kết quả mong đợi:

- ✅ Servo quay mượt đến các vị trí

- ✅ Không bị rung hoặc kêu

- ✅ Giữ vị trí ổn định

Lỗi thường gặp:

- ❌ Không quay → Quên nguồn 5V

- ❌ Quay không đúng góc → Calibration sai

- ❌ Rung/rung → Nguồn yếu

#### 7. ⚡ TEST RELAY

```cpp
// Kết nối:
// VCC → 5V
// GND → GND  
// IN → GPIO 13

void testRelay() {
    Serial.println("\n🧪 === TESTING RELAY ===");
    
    if (!relay.begin()) {
        Serial.println("❌ Relay: INIT FAILED");
        return;
    }
    
    Serial.println("✅ Relay: INIT SUCCESS");
    
    // Test 1: Bật relay
    Serial.println("🔌 Test 1: Turning relay ON");
    relay.setState(HIGH);
    Serial.println("👂 Nghe tiếng click? 💡 Đèn LED trên relay sáng?");
    delay(2000);
    
    // Test 2: Tắt relay  
    Serial.println("🔌 Test 2: Turning relay OFF");
    relay.setState(LOW);
    Serial.println("👂 Nghe tiếng click? 💡 Đèn LED trên relay tắt?");
    delay(2000);
}
```

Kết quả mong đợi:

- ✅ Nghe tiếng click rõ ràng khi bật/tắt

- ✅ Đèn LED trên relay sáng/tắt theo trạng thái

- ✅ Có thể điều khiển thiết bị AC/DC qua relay

Lỗi thường gặp:

- ❌ Không click → Chân IN nối sai

- ❌ LED không sáng → Nguồn 5V

- ❌ Không điều khiển được → Kiểm tra jumper cap trên relay

### 🚨 QUY TRÌNH DEBUG KHI GẶP LỖI

#### Bước 1: Kiểm tra vật lý

- ✅ Dây nối đúng theo diagram

- ✅ Nguồn điện đủ (3.3V/5V)

- ✅ Chia sẻ GND chung

#### Bước 2: Kiểm tra code

- ✅ Include đúng file header

- ✅ Khai báo pin đúng trong pin_config.h

- ✅ Khởi tạo driver trong begin()

#### Bước 3: Kiểm tra Serial Monitor

- ✅ Baud rate 115200

- ✅ Đọc kỹ thông báo lỗi

- ✅ Theo dõi giá trị đọc được

## 📝 BÁO CÁO KẾT QUẢ TEST

### Sau khi test từng module, em hãy báo cáo theo mẫu

## 📋 BÁO CÁO TEST MODULES

**[Ghi kết quả báo cáo vào Report1.md](./Report1.md)**

```markdown

### ✅ MODULES HOẠT ĐỘNG TỐT:

✅ : Nếu module hoạt động tốt

❌ : Nếu module không hoạt tốt, có lỗi

- [ ] OLED Display
- [ ] DS18B20 Temperature 
- [ ] DHT11 Temperature/Humidity
- [ ] BH1750 Light Sensor
- [ ] MQ135 Air Quality
- [ ] Servo Motor
- [ ] Relay

### ❌ MODULES LỖI (Mô tả chi tiết):
1. **Tên module**: [Mô tả lỗi]
   - Hiện tượng: 
   - Đã kiểm tra:
   - Cần hỗ trợ:

### 🔧 YÊU CẦU HỖ TRỢ:
- [ ] Cần hỗ trợ debug module [tên]
- [ ] Cần giải thích [vấn đề]
- [ ] Cần check code [file]
```

**Chủ động: Hãy đặt câu hỏi thật cụ thể. Thay vì `"cái này không chạy"`, hãy nói `"em đã làm A, B, C, kỳ vọng X nhưng kết quả lại là Y, đây là log/code của em"`.**

# Giải thích thêm cho code tuần 2

## 🎯 GIẢI THÍCH QUAN TRỌNG

### 1. Stack Sizes được tối ưu

- Sensor Task: 8192 bytes (đọc nhiều cảm biến)

- Display Task: 6144 bytes (xử lý OLED)

- Control Task: 6144 bytes (điều khiển thiết bị)

- Safety Task: 4096 bytes (giám sát đơn giản)

### 2. Core Allocation thông minh

- Core 0: Sensor + Safety (ưu tiên cao)

- Core 1: Display + Control (UI và điều khiển)

### 3. Error Handling toàn diện

- Kiểm tra NULL trước khi sử dụng

- Timeout cho tất cả blocking operations

- Log rõ ràng khi có lỗi

### 4. Performance Monitoring

- `printTaskStatus()` hiển thị stack usage

- Theo dõi queue utilization

- Giám sát task states

## 🔧 SỬ DỤNG TRONG MAIN.CPP

```cpp
#include "services/freertos_manager.h"

void setup() {
    // Khởi tạo FreeRTOS Manager
    if (!rtosManager.initialize()) {
        Serial.println("❌ FreeRTOS initialization failed!");
        return;
    }
    
    // Bắt đầu các task
    rtosManager.startAllTasks();
}

void loop() {
    // In trạng thái hệ thống định kỳ
    static uint32_t lastStatus = 0;
    if (millis() - lastStatus > 30000) {
        rtosManager.printTaskStatus();
        lastStatus = millis();
    }
    
    vTaskDelay(pdMS_TO_TICKS(1000));
}
```

## 🚨 DEBUG TIPS

### Khi gặp lỗi Stack Overflow

```cpp
// Tăng stack size trong createTasks()
xTaskCreatePinnedToCore(..., 12288, ...); // 12KB thay vì 8KB
```

### Khi gặp Queue Full

```cpp
// Tăng queue size trong createQueues()
sensorQueue = xQueueCreate(20, sizeof(SensorData_t)); // 20 items
```

### Khi gặp Mutex Timeout

```cpp
// Tăng timeout hoặc optimize code
if (rtosManager.takeI2CMutex(pdMS_TO_TICKS(2000))) {
    // work
    rtosManager.giveI2CMutex();
}
```

# DỰ ÁN TỔNG KẾT LẠI QUÁ TRÌNH HỌC FREERTOS

Với những gì em đã học, các kiến thức, linh kiện, cảm biến,... đã có, chúng ta sẽ xây dựng một hệ thống **Nhà thông minh thu nhỏ đa nhiệm với FreeRTOS** - một project thực tế có thể áp dụng ngay vào đời sống.

## 🎯 PROJECT PROPOSAL: SMART HOME MONITORING & CONTROL SYSTEM

### 📋 Đặc điểm kỹ thuật

- Vi điều khiển: ESP32 DEVKIT V1

- Hệ điều hành: FreeRTOS

- Framework: Arduino (PlatformIO)

- Giao tiếp: WiFi, I2C, SPI, One-Wire, UART

- Hiển thị: OLED 0.96" (I2C)

- Điều khiển từ xa: Web Interface + Firebase/Blynk

### 🏗️ KIẾN TRÚC HỆ THỐNG VỚI FREERTOS

```cpp
// Cấu trúc task trong hệ thống
typedef enum {
    TASK_PRIORITY_HIGH = 4,
    TASK_PRIORITY_MEDIUM = 3, 
    TASK_PRIORITY_LOW = 2,
    TASK_PRIORITY_IDLE = 1
} task_priority_t;

// Task handles
TaskHandle_t xSensorTask;
TaskHandle_t xDisplayTask;
TaskHandle_t xControlTask;
TaskHandle_t xNetworkTask;
TaskHandle_t xSafetyMonitorTask;
```

### 🎮 CÁC TASK CHÍNH

#### 1. Sensor Fusion Task (Ưu tiên cao)

```cpp
void vSensorTask(void *pvParameters) {
    SensorData_t sensorData;
    for(;;) {
        // Đọc đa cảm biến đồng thời
        xSemaphoreTake(xI2CMutex, portMAX_DELAY);
        sensorData.temp = readDS18B20();
        sensorData.humidity = readDHT11(); 
        sensorData.light = readBH1750();
        sensorData.airQuality = readMQ135();
        xSemaphoreGive(xI2CMutex);
        
        // Gửi dữ liệu qua queue
        xQueueSend(xSensorQueue, &sensorData, 0);
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}
```

#### 2. Display Manager Task (Ưu tiên trung bình)

```cpp
void vDisplayTask(void *pvParameters) {
    SensorData_t receivedData;
    for(;;) {
        if(xQueueReceive(xSensorQueue, &receivedData, portMAX_DELAY) == pdPASS) {
            updateOLEDDisplay(receivedData);
        }
    }
}
```

#### 3. Network Communication Task (Ưu tiên trung bình)

```cpp
void vNetworkTask(void *pvParameters) {
    for(;;) {
        // Đồng bộ với Firebase
        if(xSemaphoreTake(xWiFiMutex, pdMS_TO_TICKS(1000)) == pdTRUE) {
            syncWithFirebase();
            xSemaphoreGive(xWiFiMutex);
        }
        
        // Xử lý web commands từ queue
        processWebCommands();
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}
```

#### 4. Device Control Task (Ưu tiên cao)

```cpp
void vControlTask(void *pvParameters) {
    ControlCommand_t command;
    for(;;) {
        if(xQueueReceive(xControlQueue, &command, portMAX_DELAY) == pdPASS) {
            switch(command.device) {
                case DEVICE_RELAY:
                    digitalWrite(RELAY_PIN, command.state);
                    break;
                case DEVICE_SERVO:
                    setServoAngle(command.angle);
                    break;
                case DEVICE_FAN:
                    setFanSpeed(command.speed);
                    break;
            }
        }
    }
}
```

#### 5. Safety Monitor Task (Ưu tiên cao nhất)

```cpp
void vSafetyMonitorTask(void *pvParameters) {
    for(;;) {
        // Kiểm tra nhiệt độ khẩn cấp
        if(currentTemp > TEMP_CRITICAL) {
            xTaskNotify(xControlTask, EMERGENCY_SHUTDOWN, eSetValueWithOverwrite);
        }
        
        // Kiểm tra kết nối
        if(xTaskGetTickCount() - lastHeartbeat > HEARTBEAT_TIMEOUT) {
            emergencyReboot();
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
```

### 🔧 PHẦN CỨNG SỬ DỤNG

| Linh kiện         | Chức năng                   | Giao tiếp   |
|-------------------|-----------------------------|-------------|
| ESP32-WROOM-32    | Main Controller             | -           |
| DS18B20           | Nhiệt độ chính xác          | One-Wire    |
| DHT11             | Nhiệt độ/Độ ẩm              | Digital     |
| BH1750            | Ánh sáng                    | I2C         |
| MQ135             | Chất lượng không khí        | Analog      |
| OLED 0.96"        | Hiển thị                    | I2C         |
| Relay CW-025      | Bật/tắt thiết bị            | Digital     |
| Servo SG90        | Điều khiển góc              | PWM         |
| Quạt DC           | Thông gió                   | PWM         |
| Pulse Sensor      | Giám sát (option)           | Analog      |

### 🚀 CÁC TÍNH NĂNG THỰC TẾ

- Giám sát môi trường thời gian thực

- Điều khiển tự động:

  - Bật quạt khi nhiệt độ > 30°C

  - Đóng cửa (servo) khi trời mưa

  - Bật đèn khi trời tối

- Điều khiển từ xa qua web

- Cảnh báo an toàn

- Ghi log dữ liệu

### 📝 KẾ HOẠCH TRIỂN KHAI

#### Tuần 1

- Thiết lập project PlatformIO + FreeRTOS

- Code driver cho từng cảm biến

- Test từng sensor riêng lẻ

[Đọc nội dung thực hiện của tuần 1 tại đây](./Smart_Home_FreeRTOS/Report_Sesion01/README1.md)

#### Tuần 2

- Triển khai các task FreeRTOS

- Implement queue, semaphore, mutex

- Task communication & synchronization

#### Tuần 3

- Web interface + Firebase integration

- Control logic automation

- Safety features

#### Tuần 4

- Optimization & debugging

- Performance monitoring

- Documentation

### 🎯 MỤC TIÊU HỌC TẬP

- ✅ Lập trình đa nhiệm với FreeRTOS

- ✅ Quản lý tài nguyên (mutex, semaphore)

- ✅ Giao tiếp giữa các task (queue, notification)

- ✅ Xử lý sensor fusion

- ✅ IoT communication patterns

- ✅ Debug hệ thống nhúng phức tạp

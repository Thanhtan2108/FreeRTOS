# Tuần 2

## 📋 KẾ HOẠCH TUẦN 2

### 🎯 MỤC TIÊU CHÍNH

- Thiết kế kiến trúc FreeRTOS hoàn chỉnh

- Triển khai các task với cơ chế đồng bộ

- Tích hợp Watchdog Timer cho độ tin cậy

- Đảm bảo tính độc lập và chịu lỗi của hệ thống

## 📅 LỘ TRÌNH 7 NGÀY

### 🏗️ NGÀY 1-2: THIẾT LẬP CẤU TRÚC & KIẾN TRÚC

#### Bước 1: Tạo cấu trúc thư mục FreeRTOS

```text
Smart_Home_FreeRTOS/
├── src/
│   ├── main.cpp
│   ├── include/              # NEW
│   │   └── data_types.h
│   ├── services/             # NEW
│   │   ├── freertos_manager.h/cpp
│   │   ├── watchdog_manager.h/cpp
│   │   └── task_watchdog.h/cpp
│   ├── tasks/                # NEW
│   │   ├── sensor_task.h/cpp
│   │   ├── display_task.h/cpp
│   │   ├── control_task.h/cpp
│   │   └── safety_task.h/cpp
│   ├── drivers/              # FROM WEEK 1
│   └── config/               # FROM WEEK 1
```

#### Bước 2: Tạo data_types.h

src/include/data_types.h:

```cpp
#ifndef DATA_TYPES_H
#define DATA_TYPES_H

#include <Arduino.h>

typedef struct {
    float ds18b20_temp;
    float dht11_temp;
    float dht11_humidity;
    float bh1750_light;
    float mq135_air_quality;
    uint32_t timestamp;
} SensorData_t;

typedef struct {
    uint8_t device_type;
    uint8_t command;
    uint8_t value;
    uint32_t timestamp;
} ControlCommand_t;

typedef enum {
    DEVICE_RELAY = 0,
    DEVICE_SERVO = 1,
    DEVICE_FAN = 2
} DeviceType_t;

typedef enum {
    CMD_OFF = 0,
    CMD_ON = 1,
    CMD_SET_ANGLE = 2
} CommandType_t;

#endif
```

#### Bước 3: Cập nhật pin_config.h

src/config/pin_config.h:

```cpp
#ifndef PIN_CONFIG_H
#define PIN_CONFIG_H

// Sensor Pins
#define DS18B20_PIN      4
#define DHT11_PIN        5  
#define MQ135_PIN        34

// Actuator Pins
#define RELAY_PIN        13
#define SERVO_PIN        18
#define FAN_PWM_PIN      19
#define BUZZER_PIN       2

// I2C Pins
#define I2C_SDA          21
#define I2C_SCL          22
#define OLED_ADDRESS     0x3C

#endif
```

### 🔧 NGÀY 3-4: TRIỂN KHAI CORE MANAGERS

#### Bước 4: Triển khai FreeRTOS Manager

src/services/freertos_manager.h:

```cpp
#ifndef FREERTOS_MANAGER_H
#define FREERTOS_MANAGER_H

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <freertos/semphr.h>
#include "../include/data_types.h"

// Task priorities
typedef enum {
    TASK_PRIORITY_SAFETY = 5,
    TASK_PRIORITY_SENSOR = 4,
    TASK_PRIORITY_CONTROL = 3, 
    TASK_PRIORITY_DISPLAY = 2,
    TASK_PRIORITY_IDLE = 1
} task_priority_t;

class FreeRTOSManager {
public:
    FreeRTOSManager();
    bool initialize();
    
    // Queue methods
    bool sendSensorData(const SensorData_t& data);
    bool receiveSensorData(SensorData_t& data, TickType_t timeout = portMAX_DELAY);
    bool sendControlCommand(const ControlCommand_t& command);
    bool receiveControlCommand(ControlCommand_t& command, TickType_t timeout = portMAX_DELAY);
    
    // Mutex methods
    bool takeI2CMutex(TickType_t timeout = portMAX_DELAY);
    void giveI2CMutex();
    bool takeSerialMutex(TickType_t timeout = portMAX_DELAY);
    void giveSerialMutex();
    
    // Task management
    void startAllTasks();
    void printTaskStatus();
    
    // Public handles
    QueueHandle_t sensorQueue;
    QueueHandle_t controlQueue;
    SemaphoreHandle_t i2cMutex;
    SemaphoreHandle_t serialMutex;

private:
    void createQueues();
    void createMutexes();
    void createTasks();
};

extern FreeRTOSManager rtosManager;

#endif
```

src/services/freertos_manager.cpp:

```cpp
#include "freertos_manager.h"
#include "../config/pin_config.h"

// Task function prototypes - các hàm này được triển khai trong các file task riêng
extern "C" {
    void vSensorTask(void *pvParameters);
    void vDisplayTask(void *pvParameters);
    void vControlTask(void *pvParameters);
    void vSafetyTask(void *pvParameters);
}

// Task handles - khai báo global để có thể truy cập từ bên ngoài
TaskHandle_t xSensorTaskHandle = NULL;
TaskHandle_t xDisplayTaskHandle = NULL;
TaskHandle_t xControlTaskHandle = NULL;
TaskHandle_t xSafetyTaskHandle = NULL;

// Global instance của FreeRTOSManager
FreeRTOSManager rtosManager;

FreeRTOSManager::FreeRTOSManager() 
    : sensorQueue(NULL), 
      controlQueue(NULL), 
      i2cMutex(NULL), 
      serialMutex(NULL) {
    // Constructor - khởi tạo các handle là NULL
}

bool FreeRTOSManager::initialize() {
    Serial.println("\n🔄 Initializing FreeRTOS Manager...");
    
    // Tạo queues trước
    createQueues();
    
    // Tạo mutexes
    createMutexes();
    
    // Tạo tasks
    createTasks();
    
    // Kiểm tra tất cả thành phần đã được tạo thành công
    bool success = (sensorQueue != NULL) && 
                   (controlQueue != NULL) && 
                   (i2cMutex != NULL) && 
                   (serialMutex != NULL) &&
                   (xSensorTaskHandle != NULL) &&
                   (xDisplayTaskHandle != NULL) && 
                   (xControlTaskHandle != NULL) &&
                   (xSafetyTaskHandle != NULL);
    
    if (success) {
        Serial.println("✅ FreeRTOS Manager initialized successfully");
        Serial.println("📊 System Configuration:");
        Serial.printf("   - Sensor Queue: %s\n", sensorQueue ? "CREATED" : "FAILED");
        Serial.printf("   - Control Queue: %s\n", controlQueue ? "CREATED" : "FAILED");
        Serial.printf("   - I2C Mutex: %s\n", i2cMutex ? "CREATED" : "FAILED");
        Serial.printf("   - Serial Mutex: %s\n", serialMutex ? "CREATED" : "FAILED");
        Serial.printf("   - Sensor Task: %s\n", xSensorTaskHandle ? "CREATED" : "FAILED");
        Serial.printf("   - Display Task: %s\n", xDisplayTaskHandle ? "CREATED" : "FAILED");
        Serial.printf("   - Control Task: %s\n", xControlTaskHandle ? "CREATED" : "FAILED");
        Serial.printf("   - Safety Task: %s\n", xSafetyTaskHandle ? "CREATED" : "FAILED");
    } else {
        Serial.println("❌ FreeRTOS Manager initialization failed!");
        Serial.println("🔍 Please check the errors above");
    }
    
    return success;
}

void FreeRTOSManager::createQueues() {
    Serial.println("🔧 Creating queues...");
    
    // Tạo sensor queue - chứa 10 SensorData_t
    sensorQueue = xQueueCreate(10, sizeof(SensorData_t));
    if (sensorQueue == NULL) {
        Serial.println("❌ Failed to create sensor queue");
    } else {
        Serial.println("✅ Sensor queue created (10 items)");
    }
    
    // Tạo control queue - chứa 5 ControlCommand_t  
    controlQueue = xQueueCreate(5, sizeof(ControlCommand_t));
    if (controlQueue == NULL) {
        Serial.println("❌ Failed to create control queue");
    } else {
        Serial.println("✅ Control queue created (5 items)");
    }
}

void FreeRTOSManager::createMutexes() {
    Serial.println("🔧 Creating mutexes...");
    
    // Tạo I2C mutex - bảo vệ truy cập I2C bus
    i2cMutex = xSemaphoreCreateMutex();
    if (i2cMutex == NULL) {
        Serial.println("❌ Failed to create I2C mutex");
    } else {
        Serial.println("✅ I2C mutex created");
    }
    
    // Tạo Serial mutex - bảo vệ truy cập Serial output
    serialMutex = xSemaphoreCreateMutex();
    if (serialMutex == NULL) {
        Serial.println("❌ Failed to create Serial mutex");
    } else {
        Serial.println("✅ Serial mutex created");
    }
}

void FreeRTOSManager::createTasks() {
    Serial.println("🔧 Creating FreeRTOS tasks...");
    
    BaseType_t xReturn;
    
    // Tạo Sensor Task - chạy trên core 0
    xReturn = xTaskCreatePinnedToCore(
        vSensorTask,           // Task function
        "SensorTask",          // Task name
        8192,                  // Stack size (8KB)
        NULL,                  // Parameters
        TASK_PRIORITY_SENSOR,  // Priority
        &xSensorTaskHandle,    // Task handle
        0                      // Core 0
    );
    
    if (xReturn != pdPASS) {
        Serial.println("❌ Failed to create Sensor Task");
        xSensorTaskHandle = NULL;
    } else {
        Serial.println("✅ Sensor Task created (Core 0, Priority 4, Stack 8KB)");
    }
    
    // Tạo Display Task - chạy trên core 1
    xReturn = xTaskCreatePinnedToCore(
        vDisplayTask,
        "DisplayTask", 
        6144,                  // Stack size (6KB)
        NULL,
        TASK_PRIORITY_DISPLAY,
        &xDisplayTaskHandle,
        1                      // Core 1
    );
    
    if (xReturn != pdPASS) {
        Serial.println("❌ Failed to create Display Task");
        xDisplayTaskHandle = NULL;
    } else {
        Serial.println("✅ Display Task created (Core 1, Priority 2, Stack 6KB)");
    }
    
    // Tạo Control Task - chạy trên core 1
    xReturn = xTaskCreatePinnedToCore(
        vControlTask,
        "ControlTask",
        6144,                  // Stack size (6KB)
        NULL,
        TASK_PRIORITY_CONTROL,
        &xControlTaskHandle,
        1                      // Core 1
    );
    
    if (xReturn != pdPASS) {
        Serial.println("❌ Failed to create Control Task");
        xControlTaskHandle = NULL;
    } else {
        Serial.println("✅ Control Task created (Core 1, Priority 3, Stack 6KB)");
    }
    
    // Tạo Safety Task - chạy trên core 0 (ưu tiên cao nhất)
    xReturn = xTaskCreatePinnedToCore(
        vSafetyTask,
        "SafetyTask",
        4096,                  // Stack size (4KB)
        NULL,
        TASK_PRIORITY_SAFETY,
        &xSafetyTaskHandle,
        0                      // Core 0
    );
    
    if (xReturn != pdPASS) {
        Serial.println("❌ Failed to create Safety Task");
        xSafetyTaskHandle = NULL;
    } else {
        Serial.println("✅ Safety Task created (Core 0, Priority 5, Stack 4KB)");
    }
}

// ============================================================================
// QUEUE ACCESS METHODS
// ============================================================================

bool FreeRTOSManager::sendSensorData(const SensorData_t& data) {
    if (sensorQueue == NULL) {
        Serial.println("❌ Sensor queue not initialized");
        return false;
    }
    
    BaseType_t xStatus = xQueueSend(sensorQueue, &data, pdMS_TO_TICKS(100));
    
    if (xStatus != pdPASS) {
        // Có thể log cảnh báo nếu cần, nhưng không nên block hệ thống
        return false;
    }
    
    return true;
}

bool FreeRTOSManager::receiveSensorData(SensorData_t& data, TickType_t timeout) {
    if (sensorQueue == NULL) {
        return false;
    }
    
    return xQueueReceive(sensorQueue, &data, timeout) == pdPASS;
}

bool FreeRTOSManager::sendControlCommand(const ControlCommand_t& command) {
    if (controlQueue == NULL) {
        Serial.println("❌ Control queue not initialized");
        return false;
    }
    
    BaseType_t xStatus = xQueueSend(controlQueue, &command, pdMS_TO_TICKS(100));
    
    if (xStatus != pdPASS) {
        Serial.println("⚠️ Control queue full - command not sent");
        return false;
    }
    
    return true;
}

bool FreeRTOSManager::receiveControlCommand(ControlCommand_t& command, TickType_t timeout) {
    if (controlQueue == NULL) {
        return false;
    }
    
    return xQueueReceive(controlQueue, &command, timeout) == pdPASS;
}

// ============================================================================
// MUTEX ACCESS METHODS  
// ============================================================================

bool FreeRTOSManager::takeI2CMutex(TickType_t timeout) {
    if (i2cMutex == NULL) {
        Serial.println("❌ I2C mutex not initialized");
        return false;
    }
    
    BaseType_t xStatus = xSemaphoreTake(i2cMutex, timeout);
    
    if (xStatus != pdPASS) {
        // Timeout - không nên log ở đây vì có thể gây spam
        return false;
    }
    
    return true;
}

void FreeRTOSManager::giveI2CMutex() {
    if (i2cMutex != NULL) {
        xSemaphoreGive(i2cMutex);
    }
}

bool FreeRTOSManager::takeSerialMutex(TickType_t timeout) {
    if (serialMutex == NULL) {
        // Nếu mutex chưa được tạo, vẫn cho phép truy cập Serial
        return true;
    }
    
    BaseType_t xStatus = xSemaphoreTake(serialMutex, timeout);
    return (xStatus == pdPASS);
}

void FreeRTOSManager::giveSerialMutex() {
    if (serialMutex != NULL) {
        xSemaphoreGive(serialMutex);
    }
}

// ============================================================================
// TASK MANAGEMENT METHODS
// ============================================================================

void FreeRTOSManager::startAllTasks() {
    Serial.println("\n🚀 Starting all FreeRTOS tasks...");
    
    // Các task đã tự động start sau khi được tạo
    // Phương thức này chủ yếu để log và verify
    
    Serial.println("✅ All tasks are now running");
    Serial.println("💡 FreeRTOS scheduler is active");
}

void FreeRTOSManager::printTaskStatus() {
    Serial.println("\n" + String(50, '='));
    Serial.println("📊 FREE RTOS TASK STATUS");
    Serial.println(String(50, '='));
    
    // Sensor Task Status
    if (xSensorTaskHandle != NULL) {
        eTaskState state = eTaskGetState(xSensorTaskHandle);
        const char* stateStr = "";
        switch(state) {
            case eRunning: stateStr = "RUNNING"; break;
            case eReady: stateStr = "READY"; break;
            case eBlocked: stateStr = "BLOCKED"; break;
            case eSuspended: stateStr = "SUSPENDED"; break;
            case eDeleted: stateStr = "DELETED"; break;
            default: stateStr = "UNKNOWN"; break;
        }
        
        UBaseType_t stackHighWater = uxTaskGetStackHighWaterMark(xSensorTaskHandle);
        Serial.printf("🔍 Sensor Task: %s | Stack: %d | Core: 0\n", stateStr, stackHighWater);
    } else {
        Serial.println("🔍 Sensor Task: NOT CREATED");
    }
    
    // Display Task Status
    if (xDisplayTaskHandle != NULL) {
        eTaskState state = eTaskGetState(xDisplayTaskHandle);
        const char* stateStr = "";
        switch(state) {
            case eRunning: stateStr = "RUNNING"; break;
            case eReady: stateStr = "READY"; break;
            case eBlocked: stateStr = "BLOCKED"; break;
            case eSuspended: stateStr = "SUSPENDED"; break;
            case eDeleted: stateStr = "DELETED"; break;
            default: stateStr = "UNKNOWN"; break;
        }
        
        UBaseType_t stackHighWater = uxTaskGetStackHighWaterMark(xDisplayTaskHandle);
        Serial.printf("🔍 Display Task: %s | Stack: %d | Core: 1\n", stateStr, stackHighWater);
    } else {
        Serial.println("🔍 Display Task: NOT CREATED");
    }
    
    // Control Task Status
    if (xControlTaskHandle != NULL) {
        eTaskState state = eTaskGetState(xControlTaskHandle);
        const char* stateStr = "";
        switch(state) {
            case eRunning: stateStr = "RUNNING"; break;
            case eReady: stateStr = "READY"; break;
            case eBlocked: stateStr = "BLOCKED"; break;
            case eSuspended: stateStr = "SUSPENDED"; break;
            case eDeleted: stateStr = "DELETED"; break;
            default: stateStr = "UNKNOWN"; break;
        }
        
        UBaseType_t stackHighWater = uxTaskGetStackHighWaterMark(xControlTaskHandle);
        Serial.printf("🔍 Control Task: %s | Stack: %d | Core: 1\n", stateStr, stackHighWater);
    } else {
        Serial.println("🔍 Control Task: NOT CREATED");
    }
    
    // Safety Task Status
    if (xSafetyTaskHandle != NULL) {
        eTaskState state = eTaskGetState(xSafetyTaskHandle);
        const char* stateStr = "";
        switch(state) {
            case eRunning: stateStr = "RUNNING"; break;
            case eReady: stateStr = "READY"; break;
            case eBlocked: stateStr = "BLOCKED"; break;
            case eSuspended: stateStr = "SUSPENDED"; break;
            case eDeleted: stateStr = "DELETED"; break;
            default: stateStr = "UNKNOWN"; break;
        }
        
        UBaseType_t stackHighWater = uxTaskGetStackHighWaterMark(xSafetyTaskHandle);
        Serial.printf("🔍 Safety Task: %s | Stack: %d | Core: 0\n", stateStr, stackHighWater);
    } else {
        Serial.println("🔍 Safety Task: NOT CREATED");
    }
    
    // Queue Status
    if (sensorQueue != NULL) {
        UBaseType_t messagesWaiting = uxQueueMessagesWaiting(sensorQueue);
        UBaseType_t spacesAvailable = uxQueueSpacesAvailable(sensorQueue);
        Serial.printf("📊 Sensor Queue: %d messages, %d spaces available\n", 
                     messagesWaiting, spacesAvailable);
    } else {
        Serial.println("📊 Sensor Queue: NOT CREATED");
    }
    
    if (controlQueue != NULL) {
        UBaseType_t messagesWaiting = uxQueueMessagesWaiting(controlQueue);
        UBaseType_t spacesAvailable = uxQueueSpacesAvailable(controlQueue);
        Serial.printf("📊 Control Queue: %d messages, %d spaces available\n", 
                     messagesWaiting, spacesAvailable);
    } else {
        Serial.println("📊 Control Queue: NOT CREATED");
    }
    
    Serial.println(String(50, '='));
}

// ============================================================================
// PUBLIC TASK HANDLE ACCESS
// ============================================================================

// Các task handles đã được khai báo là public trong class
// Có thể truy cập trực tiếp từ bên ngoài, ví dụ:
// extern FreeRTOSManager rtosManager;
// TaskHandle_t sensorHandle = rtosManager.xSensorTaskHandle;

// ============================================================================
// HELPER FUNCTIONS (nếu cần thêm)
// ============================================================================

// Có thể thêm các helper functions ở đây nếu cần
// Ví dụ: hàm để suspend/resume tasks, change priorities, etc.
```

**[Đọc thêm giải thích tại đây](./Explain2.md)**

#### Bước 5: Triển khai Watchdog Manager

src/services/watchdog_manager.h:

```cpp
#ifndef WATCHDOG_MANAGER_H
#define WATCHDOG_MANAGER_H

#include <Arduino.h>
#include <esp_task_wdt.h>

class WatchdogManager {
public:
    static bool initialize(uint32_t timeoutSeconds = 10);
    static void feed();
    static void addTaskToWatch(TaskHandle_t taskHandle = NULL);
    static void removeTaskFromWatch(TaskHandle_t taskHandle = NULL);
    static void printStatus();
    
private:
    static bool initialized;
    static uint32_t watchdogTimeout;
};

#endif
```

src/services/task_watchdog.h:

```cpp
#ifndef TASK_WATCHDOG_H
#define TASK_WATCHDOG_H

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

typedef struct {
    TaskHandle_t taskHandle;
    const char* taskName;
    uint32_t lastAliveTime;
    uint32_t timeoutMs;
    uint32_t restartCount;
    bool enabled;
} TaskWatchdogItem_t;

class TaskWatchdog {
public:
    static void initialize();
    static void registerTask(TaskHandle_t task, const char* taskName, uint32_t timeoutMs);
    static void taskAlive(TaskHandle_t task);
    static void checkTasks();
    static void printStatus();
    
private:
    static TaskWatchdogItem_t tasks[10];
    static uint8_t taskCount;
    static bool initialized;
};

#endif
```

### 🎯 NGÀY 5-6: TRIỂN KHAI CÁC TASK

#### Bước 6: Triển khai Sensor Task

src/tasks/sensor_task.h:

```cpp
#ifndef SENSOR_TASK_H
#define SENSOR_TASK_H

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

void vSensorTask(void *pvParameters);

#endif
```

src/tasks/sensor_task.cpp:

```cpp
#include "sensor_task.h"
#include "../services/freertos_manager.h"
#include "../services/task_watchdog.h"
#include "../services/watchdog_manager.h"
#include "../drivers/ds18b20_driver/ds18b20_driver.h"
#include "../drivers/dht11_driver/dht11_driver.h"
#include "../drivers/bh1750_driver/bh1750_driver.h"
#include "../drivers/mq135_driver/mq135_driver.h"

extern DS18B20Driver tempSensor;
extern DHT11Driver dhtSensor;
extern BH1750Driver lightSensor;
extern MQ135Driver airQualitySensor;

void vSensorTask(void *pvParameters) {
    (void) pvParameters;
    
    Serial.println("✅ Sensor Task started");
    TaskWatchdog::registerTask(xTaskGetCurrentTaskHandle(), "SensorTask", 15000);
    
    SensorData_t sensorData;
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(5000);
    
    // Sensor health tracking
    bool ds18b20_healthy = true;
    bool dht11_healthy = true;
    bool bh1750_healthy = true;
    bool mq135_healthy = true;

    for(;;) {
        TaskWatchdog::taskAlive(xTaskGetCurrentTaskHandle());
        
        memset(&sensorData, 0, sizeof(SensorData_t));
        sensorData.timestamp = millis();
        
        // Read DS18B20 with error handling
        if (ds18b20_healthy && rtosManager.takeI2CMutex(pdMS_TO_TICKS(1000))) {
            float temp = tempSensor.readTemperature();
            if (temp != -127.0f) {
                sensorData.ds18b20_temp = temp;
            } else {
                sensorData.ds18b20_temp = -999.0f;
                ds18b20_healthy = false;
                Serial.println("❌ DS18B20 read error");
            }
            rtosManager.giveI2CMutex();
        }
        
        // Read DHT11 with error handling  
        if (dht11_healthy && rtosManager.takeI2CMutex(pdMS_TO_TICKS(1000))) {
            float temp = dhtSensor.readTemperature();
            float humidity = dhtSensor.readHumidity();
            if (temp != -127.0f && humidity != -1.0f) {
                sensorData.dht11_temp = temp;
                sensorData.dht11_humidity = humidity;
            } else {
                sensorData.dht11_temp = -999.0f;
                sensorData.dht11_humidity = -999.0f;
                dht11_healthy = false;
                Serial.println("❌ DHT11 read error");
            }
            rtosManager.giveI2CMutex();
        }
        
        // Send data regardless of errors
        if (rtosManager.sendSensorData(sensorData)) {
            Serial.println("📊 Sensor data sent to queue");
        }
        
        WatchdogManager::feed();
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}
```

#### Bước 7: Triển khai Display Task

src/tasks/display_task.h:

```cpp
#ifndef DISPLAY_TASK_H
#define DISPLAY_TASK_H

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

void vDisplayTask(void *pvParameters);

#endif
```

src/tasks/display_task.cpp:

```cpp
#include "display_task.h"
#include "../services/freertos_manager.h"
#include "../services/task_watchdog.h"
#include "../services/watchdog_manager.h"
#include "../drivers/oled_driver/oled_driver.h"

extern OLEDDriver oled;

void vDisplayTask(void *pvParameters) {
    (void) pvParameters;
    
    Serial.println("✅ Display Task started");
    TaskWatchdog::registerTask(xTaskGetCurrentTaskHandle(), "DisplayTask", 15000);
    
    SensorData_t sensorData;
    
    for(;;) {
        TaskWatchdog::taskAlive(xTaskGetCurrentTaskHandle());
        
        if (rtosManager.receiveSensorData(sensorData, portMAX_DELAY)) {
            oled.displaySensorData(
                sensorData.ds18b20_temp,
                sensorData.dht11_humidity, 
                sensorData.bh1750_light,
                sensorData.mq135_air_quality
            );
            
            Serial.printf("🖥️ Display updated - Temp: %.1fC\n", sensorData.ds18b20_temp);
        }
        
        WatchdogManager::feed();
    }
}
```

#### Bước 8: Triển khai Safety Task

src/tasks/safety_task.h:

```cpp
#ifndef SAFETY_TASK_H
#define SAFETY_TASK_H

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

void vSafetyTask(void *pvParameters);

#endif
```

src/tasks/safety_task.cpp:

```cpp
#include "safety_task.h"
#include "../services/freertos_manager.h"
#include "../services/task_watchdog.h"
#include "../services/watchdog_manager.h"

void vSafetyTask(void *pvParameters) {
    (void) pvParameters;
    
    Serial.println("✅ Safety Task started");
    TaskWatchdog::registerTask(xTaskGetCurrentTaskHandle(), "SafetyTask", 10000);
    
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(2000);
    
    for(;;) {
        TaskWatchdog::taskAlive(xTaskGetCurrentTaskHandle());
        
        // Check all tasks with software watchdog
        TaskWatchdog::checkTasks();
        
        // Print system status periodically
        static uint32_t lastStatusPrint = 0;
        if (millis() - lastStatusPrint > 30000) {
            if (rtosManager.takeSerialMutex(pdMS_TO_TICKS(100))) {
                Serial.println("\n=== SYSTEM HEALTH STATUS ===");
                TaskWatchdog::printStatus();
                WatchdogManager::printStatus();
                rtosManager.giveSerialMutex();
                lastStatusPrint = millis();
            }
        }
        
        WatchdogManager::feed();
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}
```

### 🏁 NGÀY 7: TÍCH HỢP & TESTING

#### Bước 9: Cập nhật main.cpp

src/main.cpp:

```cpp
#include <Arduino.h>

// Include drivers
#include "drivers/ds18b20_driver/ds18b20_driver.h"
#include "drivers/dht11_driver/dht11_driver.h" 
#include "drivers/oled_driver/oled_driver.h"
#include "drivers/bh1750_driver/bh1750_driver.h"
#include "drivers/mq135_driver/mq135_driver.h"
#include "drivers/servo_driver/servo_driver.h"
#include "drivers/relay_driver/relay_driver.h"

// Include FreeRTOS tasks
#include "tasks/sensor_task.h"
#include "tasks/display_task.h"
#include "tasks/control_task.h"
#include "tasks/safety_task.h"

// Include managers
#include "services/freertos_manager.h"
#include "services/watchdog_manager.h"
#include "services/task_watchdog.h"

// Global driver instances
DS18B20Driver tempSensor;
DHT11Driver dhtSensor;
OLEDDriver oled;
BH1750Driver lightSensor;
MQ135Driver airQualitySensor;
ServoDriver servo;
RelayDriver relay;

void initializeDrivers() {
    Serial.println("🔧 Initializing all drivers...");
    
    tempSensor.begin();
    dhtSensor.begin();
    oled.begin();
    lightSensor.begin();
    airQualitySensor.begin();
    servo.begin();
    relay.begin();
    
    Serial.println("✅ All drivers initialized");
}

void setup() {
    Serial.begin(115200);
    delay(2000);
    
    Serial.println("\n" + String(50, '='));
    Serial.println("🚀 SMART HOME SYSTEM - FREERTOS + WATCHDOG");
    Serial.println("📅 Phase: Week 2 - Complete Integration");
    Serial.println(String(50, '='));
    
    // Step 1: Initialize drivers
    initializeDrivers();
    
    // Step 2: Initialize Watchdog FIRST
    if (!WatchdogManager::initialize(15)) {
        Serial.println("❌ CRITICAL: Watchdog initialization failed!");
    }
    TaskWatchdog::initialize();
    
    // Step 3: Initialize FreeRTOS manager
    if (!rtosManager.initialize()) {
        Serial.println("❌ CRITICAL: FreeRTOS initialization failed!");
        while(1) { 
            WatchdogManager::feed();
            delay(1000); 
        }
    }
    
    // Step 4: Start all tasks
    rtosManager.startAllTasks();
    
    // Show initial status
    oled.displaySystemStatus("FreeRTOS Ready!");
    delay(2000);
    
    Serial.println("✅ System initialization completed");
    Serial.println("💡 System is now running with FreeRTOS + Watchdog protection");
}

void loop() {
    // Main loop only feeds watchdog and monitors
    WatchdogManager::feed();
    
    // Print heartbeat every 30 seconds
    static uint32_t lastHeartbeat = 0;
    if (millis() - lastHeartbeat > 30000) {
        Serial.println("💓 Main loop heartbeat - System normal");
        lastHeartbeat = millis();
    }
    
    vTaskDelay(pdMS_TO_TICKS(1000));
}
```

#### Bước 10: Cập nhật platformio.ini

platformio.ini:

```cpp
[env:doit-espressif32]
platform = espressif32
board = doit-devkit-v1
framework = arduino
monitor_speed = 115200

lib_deps = 
    adafruit/Adafruit SSD1306@^2.5.7
    adafruit/Adafruit GFX Library@^1.11.5
    adafruit/Adafruit BH1750@^1.3.0
    milesburton/DallasTemperature@^3.9.0
    adafruit/Adafruit Unified Sensor@^1.1.7
    bblanchon/ArduinoJson@^6.21.3
    clawrick/ESP32Servo@^1.1.0

build_flags = 
    -D CORE_DEBUG_LEVEL=1
    -Wall
    -Werror

monitor_filters = 
    log2file
    time
```

## 🧪 TESTING PROCEDURE

### Test 1: Basic System Operation

```cpp
// Expected Serial Output:
✅ All drivers initialized
✅ FreeRTOS Manager initialized  
✅ All tasks created successfully
✅ System initialization completed
📊 Sensor data sent to queue
🖥️ Display updated - Temp: 25.5C
```

### Test 2: Error Handling Simulation

```cpp
// Disconnect DS18B20, expected behavior:
❌ DS18B20 read error
📊 Sensor data sent to queue (with error values)
🖥️ Display updated - Temp: -999.0C (but system continues)
```

### Test 3: Watchdog Test

```cpp
// Simulate Sensor Task hang by adding infinite loop
void vSensorTask(void *pvParameters) {
    // ... normal code ...
    
    // SIMULATE HANG (remove in production)
    if (millis() > 30000) { // After 30 seconds
        while(1) { /* Infinite loop */ }
    }
    
    // Expected behavior:
    // 🚨 TASK WATCHDOG: SensorTask is not responding!
    // 🔄 Restarting task: SensorTask
    // ✅ Task SensorTask restarted successfully
}
```

### Test 4: Memory & Performance

```cpp
// Monitor stack usage
void checkStackUsage() {
    Serial.printf("Sensor Task Stack: %d\n", 
                 uxTaskGetStackHighWaterMark(xSensorTaskHandle));
    Serial.printf("Display Task Stack: %d\n", 
                 uxTaskGetStackHighWaterMark(xDisplayTaskHandle));
}
```

## 📊 CRITICAL SUCCESS FACTORS

### ✅ Đảm bảo tính độc lập

- Mỗi task chạy trong không gian riêng

- Lỗi một task không ảnh hưởng task khác

- Queue và mutex ngăn chặn resource conflict

### ✅ Đảm bảo độ tin cậy

- Hardware WDT: Reset hệ thống nếu treo hoàn toàn

- Software WDT: Restart từng task nếu bị treo

- Error handling: Continue operation despite sensor failures

### ✅ Đảm bảo hiệu năng

- Task priorities hợp lý

- Stack sizes tối ưu

- Mutex timeout tránh deadlock

## 🚨 TROUBLESHOOTING COMMON ISSUES

### Issue 1: Stack Overflow

```cpp
// Solution: Increase stack size
xTaskCreate(..., 8192, ...); // Instead of 4096
```

### Issue 2: Mutex Deadlock

```cpp
// Solution: Always use timeout
if (rtosManager.takeI2CMutex(pdMS_TO_TICKS(1000))) {
    // Do work
    rtosManager.giveI2CMutex();
}
```

### Issue 3: Watchdog Timeout

```cpp
// Solution: Ensure frequent feeding
void vSensorTask(...) {
    for(;;) {
        TaskWatchdog::taskAlive(xTaskGetCurrentTaskHandle());
        WatchdogManager::feed();
        // ... work ...
        vTaskDelay(...);
    }
}
```

## 🎯 KẾT QUẢ CUỐI TUẦN 2

### Sau khi hoàn thành tuần 2, em sẽ có

- ✅ Hệ thống FreeRTOS hoàn chỉnh với 4 task độc lập

- ✅ Cơ chế đồng bộ với queue và mutex

- ✅ Watchdog protection 2 lớp (hardware + software)

- ✅ Error handling và automatic recovery

- ✅ System monitoring và health reporting

- ✅ Documentation và testing procedures

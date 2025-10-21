#include <Arduino.h>

// include library
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

// define, config

// threshold

// variable global

// initialize Mutex
SemaphoreHandle_t xMutex = NULL;

// task handle for task
TaskHandle_t xTask1Handle = NULL;
TaskHandle_t xTask2Handle = NULL;

// put function declarations here:
void vTask1(void *pvParameters);
void vTask2(void *pvParameters);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(100);
  Serial.println("\n=== FreeRTOS Mutex Example ===");

  // create mutex and check
  xMutex = xSemaphoreCreateMutex();

  if (xMutex == NULL) {
    Serial.println("❌ Failed to create Mutex!");
    while (1); // Stop if failed
  } else {
    Serial.println("✅ Mutex created successfully.");
  }

  // create task
  xTaskCreate(vTask1, "vTask1", 2048, NULL, 2, &xTask1Handle);
  xTaskCreate(vTask2, "vTask2", 2048, NULL, 2, &xTask2Handle);
}

void loop() {
  // put your main code here, to run repeatedly:
  // nothing
}

// put function definitions here:
// task1
void vTask1(void *pvParameters) {
  (void) pvParameters; // Để “tắt cảnh báo unused parameter”
  
  //stage setup

  // stage loop
  while (1) {
    // Try to take mutex (wait up to 1s)
    if (xSemaphoreTake(xMutex, pdMS_TO_TICKS(1000)) == pdTRUE) { // có thể thay timeout pdMS_TO_TICKS(1000) bằng portMAX_DELAY để đứng chờ vô hạn cho đến khi có sự kiện xảy ra
      // Vào task
      Serial.println("[Task1] Entering critical section.");

      // Giả lập công việc cần bảo vệ bằng Mutex
      vTaskDelay(pdMS_TO_TICKS(200)); // Giả sử mất 200ms để xử lý

      // Rời task
      Serial.println("[Task1] Leaving critical section.");

      // Give back mutex
      xSemaphoreGive(xMutex);
    } else {
      Serial.println("[Task1] Failed to acquire mutex (timeout).");
    }
    vTaskDelay(pdMS_TO_TICKS(1000));  // Tần suất task được chạy
  }
}

// task2
void vTask2(void *pvParameters) {
  (void) pvParameters; // Để “tắt cảnh báo unused parameter”
  
  // stage setup

  // stage loop
  while (1) {
    // Try to take mutex (wait up to 1s)
    if (xSemaphoreTake(xMutex, pdMS_TO_TICKS(1000)) == pdTRUE) { // có thể thay timeout pdMS_TO_TICKS(1000) bằng portMAX_DELAY để đứng chờ vô hạn cho đến khi có sự kiện xảy ra
      // Vào task
      Serial.println("[Task2] Entering critical section.");

      // Giả lập xử lý task trong Critial sestion
      vTaskDelay(pdMS_TO_TICKS(300)); // Giả lập xử lý task trong 300ms

      // Rời task
      Serial.println("[Task2] Leaving critical section.");

      // Give back mutex
      xSemaphoreGive(xMutex);
    } else {
      Serial.println("[Task2] Failed to acquire mutex (timeout).");
    }
    vTaskDelay(pdMS_TO_TICKS(500));  // Tần suất task được chạy
  }
}

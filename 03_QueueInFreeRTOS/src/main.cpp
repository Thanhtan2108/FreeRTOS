#include <Arduino.h>

// Include library
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

// define, config
#define LED_PIN  2

// Threshold
#define QUEUE_LENGTH  5  // Số phần tử tối đa trong Queue

// Variable global

// initialize Queue to save items
QueueHandle_t dataQueue;   // Handle của Queue

// Task handle for Task
TaskHandle_t xSenderTaskHandle = NULL;
TaskHandle_t xReceiverTaskHandle = NULL;

// put function declarations here:
void vSenderTask(void *pvParameters);
void vReceiverTask(void *pvParameters);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(100);
  Serial.println("\n=== FreeRTOS Queue Example ===");

  pinMode(LED_PIN, OUTPUT);

  // Create Queue and check
  dataQueue = xQueueCreate(QUEUE_LENGTH, sizeof(int));
  
  if (dataQueue == NULL) {
    Serial.println("❌ Failed to create Queue!");
    while (1); // Dừng lại
  } else {
    Serial.println("✅ Queue created successfully.");
  }

  // Create task
  xTaskCreate(vSenderTask, "vSenderTask", 2048, NULL, 1, &xSenderTaskHandle);
  xTaskCreate(vReceiverTask, "vReceiverTask", 2048, NULL, 1, &xReceiverTaskHandle);
}

void loop() {
  // put your main code here, to run repeatedly:
  // nothing
}

// put function definitions here:
// task send input item data from task into Queue
void vSenderTask(void *pvParameters) {
  (void) pvParameters; // Để “tắt cảnh báo unused parameter”

  // stage setup
  int counter = 0; // counter is item Queue send outsides

  // stage loop
  while (1) {
    counter++;

    // Gửi giá trị counter từ task vào Queue
    if (xQueueSend(dataQueue, &counter, pdMS_TO_TICKS(100)) == pdPASS) {
      Serial.printf("[Sender] Sent: %d\n", counter);
    } else {
      Serial.println("[Sender] Queue Full!");
    }

    vTaskDelay(pdMS_TO_TICKS(1000)); // Gửi mỗi 1s
  }
}

// task receive output item data from Queue to task other
void vReceiverTask(void *pvParameters) {
  (void) pvParameters; // Để “tắt cảnh báo unused parameter”

  // stage setup
  int receivedValue = 0; // receivedValue is item Queue receive from task 

  // stage loop
  while (1) {
    // Nhận dữ liệu từ Queue về task (chờ tối đa 2s)
    if (xQueueReceive(dataQueue, &receivedValue, pdMS_TO_TICKS(2000)) == pdPASS) {
      Serial.printf("[Receiver] Got: %d\n", receivedValue);

      // Mỗi lần nhận được dữ liệu → bật LED
      digitalWrite(LED_PIN, HIGH);
      vTaskDelay(pdMS_TO_TICKS(200));
      digitalWrite(LED_PIN, LOW);
    } else {
      Serial.println("[Receiver] Timeout waiting for data...");
    }
  }
}

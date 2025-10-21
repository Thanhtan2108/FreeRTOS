#include <Arduino.h>

// include library
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

// define, config
#define LED_BLINK_PIN 7
#define LED_TOGGLE_PIN 9
#define BUTTON_PIN 13

// threshold
constexpr uint32_t DEBOUNCE_TIME_MS = 200;

// variable global
volatile unsigned long lastInterruptTime = 0;

// initialize Semaphore
SemaphoreHandle_t xButtonSemaphore = NULL;

// task handle for task
TaskHandle_t xBlinkTaskHandle = NULL;
TaskHandle_t xToggleTaskHandle = NULL;

// put function declarations here:
void vBlinkLedTask(void *pvParameters);
void vToggleLedTask(void *pvParameters);
void IRAM_ATTR vButtonISR();
void IRAM_ATTR vGiveButtonSemaphoreFromISR();
void vSetupButtonInterrupt(uint8_t pinButton);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(100);

  // create semaphore and check
  xButtonSemaphore = xSemaphoreCreateBinary();
  if (xButtonSemaphore == NULL) {
    Serial.println("❌ Failed to create semaphore!");
    while (1);
  }

  // Setup Interrupt
  vSetupButtonInterrupt(BUTTON_PIN);

  // create task
  xTaskCreate(vBlinkLedTask, "BlinkTask", 2048, NULL, 1, &xBlinkTaskHandle);
  xTaskCreate(vToggleLedTask, "ToggleTask", 2048, NULL, 1, &xToggleTaskHandle);
  
  Serial.println("✅ Tasks and Semaphore initialized.");
}

void loop() {
  // put your main code here, to run repeatedly:
  // nothing
}

// put function definitions here:
// task blink led 0.5s
void vBlinkLedTask(void *pvParameters) {
  (void) pvParameters; // Để “tắt cảnh báo unused parameter”

  // stage setup
  pinMode(LED_BLINK_PIN, OUTPUT);

  // stage loop
  while (1) {
    digitalWrite(LED_BLINK_PIN, !digitalRead(LED_BLINK_PIN));
    vTaskDelay(pdMS_TO_TICKS(500)); // 0.5s
  }
}

// task toggle led when take semaphore from semaphore resulting from press button
void vToggleLedTask(void *pvParameters) {
  (void) pvParameters; // Để “tắt cảnh báo unused parameter”

  // stage setup
  pinMode(LED_TOGGLE_PIN, OUTPUT);

  // stage loop
  while (1) {
    // Đợi đến khi ISR give semaphore
    if (xSemaphoreTake(xButtonSemaphore, portMAX_DELAY) == pdTRUE) {
      // Toggle LED GPIO9
      digitalWrite(LED_TOGGLE_PIN, !digitalRead(LED_TOGGLE_PIN));
      Serial.println("[Task] Button pressed -> LED9 toggled!");
    }
  }
}

// Interrupt for button
void IRAM_ATTR vButtonISR() {
  // debouce button
  unsigned long currentTime = millis();

  if (currentTime - lastInterruptTime > DEBOUNCE_TIME_MS) {
    // complete deboucing for button, give semaphore from interrupt button to Semaphore in FreeRTOS
    vGiveButtonSemaphoreFromISR();

    lastInterruptTime = currentTime;
  }
}

void IRAM_ATTR vGiveButtonSemaphoreFromISR() {
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;

  // Give semaphore từ ngắt
  xSemaphoreGiveFromISR(xButtonSemaphore, &xHigherPriorityTaskWoken);

  // Cho phép chuyển context ngay nếu cần
  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void vSetupButtonInterrupt(uint8_t pinButton) {
  pinMode(pinButton, INPUT_PULLUP);
  
  // Gán ISR cho nút nhấn (kích hoạt khi nhấn xuống)
  attachInterrupt(digitalPinToInterrupt(pinButton), vButtonISR, FALLING);
}

// Đọc giải thích thêm tại explain.md

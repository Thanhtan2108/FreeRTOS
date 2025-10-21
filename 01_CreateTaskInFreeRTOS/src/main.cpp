#include <Arduino.h>

// include library
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

// define, config
#define ledRedPin 4
#define ledBluePin 5
#define buttonPin 13

// Threshold

// variable global

// Task handle cho Task
TaskHandle_t xBlinkLedHandle = NULL;
TaskHandle_t xButtonClickHandle = NULL;

// put function declarations here:
void vBlinkLedTask(void *pvParameters);
void vButtonClickTask(void *pvParameters);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(100);

  // Create task
  xTaskCreate(vBlinkLedTask, "vBlinkLedTask", 200, NULL, 1, &xBlinkLedHandle);
  xTaskCreate(vButtonClickTask, "vButtonClickTask", 500, NULL, 1, &xButtonClickHandle);
}

void loop() {
  // put your main code here, to run repeatedly:
  // nothing
}

// put function definitions here:
// task blink led red
void vBlinkLedTask(void *pvParameters) {
  (void) pvParameters; // Để “tắt cảnh báo unused parameter”

  // stage setup
  pinMode(ledRedPin, OUTPUT);
  digitalWrite(ledRedPin, LOW);

  // stage loop
  while(1) {
    digitalWrite(ledRedPin, digitalRead(ledRedPin)^1);
    Serial.println("Task vBlinkLedTask active");
    vTaskDelay(500/portTICK_PERIOD_MS);
  }
}

// task press button -> blink led blue   
void vButtonClickTask(void *pvParameters) {
  (void) pvParameters; // Để “tắt cảnh báo unused parameter”

  //stage setup
  pinMode(ledBluePin, OUTPUT);
  digitalWrite(ledBluePin, LOW);

  pinMode(buttonPin, INPUT_PULLUP);
  digitalWrite(buttonPin, HIGH);

  // stage loop
  while(1) {
    if (digitalRead(buttonPin) == LOW) {
      digitalWrite(ledBluePin, digitalRead(ledBluePin)^1);
      Serial.println("Task vButtonClickTask active");
      vTaskDelay(500/portTICK_PERIOD_MS);
    }
  }
}
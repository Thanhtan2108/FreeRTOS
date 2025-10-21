#include <Arduino.h>

// include library
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>
#include <esp_task_wdt.h>
#include <esp_system.h>

// define, config
#define buttonDropPin 13
#define ledDropPin 4

#define buttonHoldPin 14
#define ledHoldPin    5

// Threshold
#define WDT_TIMEOUT 2 // timeout 2s for WDT

// Variable global

// Task handle cho Task
TaskHandle_t xPressDropButtonBlinkLedHandle = NULL;
TaskHandle_t xPressHoldButtonBrightLedHandle = NULL;

// put function declarations here:
void vPressDropButtonBlinkLedTask (void *pvParameters);
void vPressHoldButtonBrightLedTask (void *pvParameters);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(100);
  
  // Create Task
  xTaskCreate(vPressDropButtonBlinkLedTask, "vPressDropButtonBlinkLedTask", 500, NULL, 2, &xPressDropButtonBlinkLedHandle);
  xTaskCreate(vPressHoldButtonBrightLedTask, "vPressHoldButtonBrightLedTask", 500, NULL, 2, &xPressHoldButtonBrightLedHandle);
}

void loop() {
  // put your main code here, to run repeatedly:
  // nothing
}

// put function definitions here:
// task press drop button, logic level change HIGH -> LOW
void vPressDropButtonBlinkLedTask (void *pvParameters) {
  (void) pvParameters; // Để “tắt cảnh báo unused parameter”

  // stage setup
  pinMode(ledDropPin, OUTPUT);
  digitalWrite(ledDropPin, LOW);

  pinMode(buttonDropPin, INPUT_PULLUP);
  digitalWrite(buttonDropPin, HIGH);

  bool lastStateButton = HIGH;

  // Stage loop
  while(1) {
    bool currentStateButton = digitalRead(buttonDropPin);

    if (lastStateButton == LOW && currentStateButton == HIGH) {
      digitalWrite(ledDropPin, digitalRead(ledDropPin)^1);
      vTaskDelay(500/portTICK_PERIOD_MS);
      Serial.println("Task vPressDropButtonBlinkLedTask active");
    }

    lastStateButton = currentStateButton;
    vTaskDelay(pdMS_TO_TICKS(50));
  }
}

// task press and hold button, bright led when logic level of button is HIGH
// Have use WDT
void vPressHoldButtonBrightLedTask (void *pvParameters) {
  (void) pvParameters; // Để “tắt cảnh báo unused parameter”

  // stage setup
  pinMode(ledHoldPin, OUTPUT);
  digitalWrite(ledHoldPin, LOW);

  pinMode(buttonHoldPin, INPUT_PULLUP);
  digitalWrite(buttonHoldPin, HIGH); 

  // Đăng ký WDT cho task này
  esp_task_wdt_add(NULL);

  // stage loop
  while(1) {
    bool stateButton = digitalRead(buttonHoldPin);

    if (stateButton == LOW) {
      digitalWrite(ledHoldPin, HIGH);
      Serial.println("Task vPressHoldButtonBrightLedTask active");

      // Cập nhật WDT để không timeout
      esp_task_wdt_reset();
    } else {
      digitalWrite(ledHoldPin, LOW);
      // Nếu không nhấn thì vẫn reset để không bị WDT reset task
      esp_task_wdt_reset();
    }

    vTaskDelay(pdMS_TO_TICKS(50));   
  }
}
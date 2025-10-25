
# FreeRTOS Cheatsheet — Detailed Guide (Tasks, Queues, Semaphores, Mutex, Notifications, Timers)

> Purpose: a single, portable cheatsheet for PlatformIO / Arduino (ESP32) projects that explains important FreeRTOS APIs, how they work, usage patterns, and short examples. Copy snippets into your project and experiment.

---

## Quick facts / macros

- `pdMS_TO_TICKS(ms)` — convert milliseconds to RTOS ticks.
- `portMAX_DELAY` — block indefinitely (when allowed).
- `pdTRUE` / `pdFALSE`, `pdPASS` — return status macros.
- `TickType_t` — type for ticks.
- `xTaskGetTickCount()` / `xTaskGetTickCountFromISR()` — current tick count.

---

## 1. Tasks (creation, scheduling, management)

### Key APIs Tasks

```c
BaseType_t xTaskCreate(TaskFunction_t pvTaskCode,
                       const char * const pcName,
                       configSTACK_DEPTH_TYPE usStackDepth,
                       void *pvParameters,
                       UBaseType_t uxPriority,
                       TaskHandle_t *pxCreatedTask);

BaseType_t xTaskCreatePinnedToCore(..., BaseType_t xCoreID); // ESP32
void vTaskDelete(TaskHandle_t xTaskToDelete); // NULL deletes calling task
void vTaskDelay(TickType_t xTicksToDelay);
void vTaskDelayUntil(TickType_t *pxPreviousWakeTime, TickType_t xTimeIncrement);
void vTaskSuspend(TaskHandle_t xTaskToSuspend);
void vTaskResume(TaskHandle_t xTaskToResume);
void vTaskPrioritySet(TaskHandle_t xTask, UBaseType_t uxNewPriority);
TaskHandle_t xTaskGetCurrentTaskHandle(void);
const char *pcTaskGetName(TaskHandle_t xTask);
UBaseType_t uxTaskGetStackHighWaterMark(TaskHandle_t xTask);
```

### Notes Tasks

- Task prototype: `void vTask(void *pvParameters)`.
- Use `(void) pvParameters;` inside task when not used to avoid compiler warnings.
- Priorities: higher numbers = higher priority.
- Scheduler: preemptive by default. On single-core, concurrency = time-slicing.

### Minimal example

```c
void vBlink(void *pv) {
  (void)pv;
  pinMode(2, OUTPUT);
  for(;;) {
    digitalWrite(2, !digitalRead(2));
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

xTaskCreate(vBlink, "blink", 2048, NULL, 1, NULL);
```

---

## 2. Queues (message passing & synchronization)

### Key APIs Queues

```c
QueueHandle_t xQueueCreate(UBaseType_t uxQueueLength, UBaseType_t uxItemSize);
BaseType_t xQueueSend(QueueHandle_t xQueue, const void *pvItem, TickType_t xTicksToWait);
BaseType_t xQueueSendToFront(...);
BaseType_t xQueueReceive(QueueHandle_t xQueue, void *pvBuffer, TickType_t xTicksToWait);
BaseType_t xQueuePeek(...);
UBaseType_t uxQueueMessagesWaiting(QueueHandle_t xQueue);
void vQueueDelete(QueueHandle_t xQueue);

// From ISR
BaseType_t xQueueSendFromISR(QueueHandle_t xQueue, const void *pvItem, BaseType_t *pxHigherPriorityTaskWoken);
BaseType_t xQueueReceiveFromISR(QueueHandle_t xQueue, void *pvBuffer, BaseType_t *pxHigherPriorityTaskWoken);
portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
```

### Notes Queues

- Queue stores **copies** of items (unless you send pointers intentionally).
- Choose `uxItemSize` correctly (e.g., `sizeof(int)` or `sizeof(MyStruct)`).
- Good for producer/consumer patterns; can block on send/receive until space/data is available.

### Example: simple producer/consumer

```c
QueueHandle_t q = xQueueCreate(5, sizeof(int));
int value = 1;
xQueueSend(q, &value, pdMS_TO_TICKS(100));
int rx;
if (xQueueReceive(q, &rx, portMAX_DELAY) == pdPASS) { /* use rx */ }
```

---

## 3. Semaphores (Binary & Counting)

### Create

```c
SemaphoreHandle_t xSemaphoreCreateBinary(void);
SemaphoreHandle_t xSemaphoreCreateCounting(UBaseType_t uxMaxCount, UBaseType_t uxInitialCount);

// Static (no heap)
StaticSemaphore_t xSemBuf;
SemaphoreHandle_t xSemaphoreCreateBinaryStatic(StaticSemaphore_t *pxBuffer);
```

### Key APIs Semaphore

```c
BaseType_t xSemaphoreTake(SemaphoreHandle_t xSemaphore, TickType_t xTicksToWait);
BaseType_t xSemaphoreGive(SemaphoreHandle_t xSemaphore);
BaseType_t xSemaphoreGiveFromISR(SemaphoreHandle_t xSemaphore, BaseType_t *pxHigherPriorityTaskWoken);
void vSemaphoreDelete(SemaphoreHandle_t xSemaphore);
```

### Notes

- **Binary semaphore**: good for signaling (ISR → task).
- **Counting semaphore**: manage resource pools.
- Use `FromISR` variants inside ISRs.
- `xSemaphoreCreateBinary()` often returns semaphore in **empty** state — call `xSemaphoreGive()` if you want it initially available.

### ISR → Task signaling (example)

```c
// In ISR:
BaseType_t hpw = pdFALSE;
xSemaphoreGiveFromISR(binSem, &hpw);
portYIELD_FROM_ISR(hpw);

// In Task:
if (xSemaphoreTake(binSem, portMAX_DELAY) == pdTRUE) { /* handle event */ }
```

---

## 4. Mutex (for mutual exclusion between tasks)

### Create & use

```c
SemaphoreHandle_t xSemaphoreCreateMutex(void);
BaseType_t xSemaphoreTake(SemaphoreHandle_t xMutex, TickType_t xTicksToWait);
BaseType_t xSemaphoreGive(SemaphoreHandle_t xMutex);
void vSemaphoreDelete(SemaphoreHandle_t xMutex);
```

### Notes Mutex

- Mutex is a special semaphore with **priority inheritance** and **ownership**.
- **Do not use mutex in ISR** — mutex makes no sense in ISR context and has no FromISR APIs.
- Use mutex to protect shared resources (Serial, I2C bus, shared variables).

### Example: protect Serial

```c
if (xSemaphoreTake(serialMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
  Serial.print("safe ");
  xSemaphoreGive(serialMutex);
}
```

---

## 5. Task Notifications (lightweight signaling 1:1)

### Key APIs Task Notifications

```c
void vTaskNotifyGive(TaskHandle_t xTaskToNotify);
BaseType_t xTaskNotifyGiveFromISR(TaskHandle_t xTaskToNotify, BaseType_t *pxHigherPriorityTaskWoken);
uint32_t ulTaskNotifyTake(BaseType_t xClearCountOnExit, TickType_t xTicksToWait);

BaseType_t xTaskNotify(TaskHandle_t xTaskToNotify, uint32_t ulValue, eNotifyAction eAction);
BaseType_t xTaskNotifyWait(uint32_t ulBitsToClearOnEntry, uint32_t ulBitsToClearOnExit,
                           uint32_t *pulNotificationValue, TickType_t xTicksToWait);
```

### Notes Task Notifications

- Faster than semaphore for 1:1 notification.
- No heap required; stored in TCB.
- Can carry 32-bit value or act as simple event count.

### Example

```c
// Sender:
vTaskNotifyGive(receiverHandle);

// Receiver:
ulTaskNotifyTake(pdTRUE, portMAX_DELAY); // blocks until notified
```

---

## 6. Event Groups (multi-bit sync for multiple tasks)

### Key APIs Event Group

```c
EventGroupHandle_t xEventGroupCreate(void);
EventBits_t xEventGroupSetBits(EventGroupHandle_t xEventGroup, EventBits_t uxBitsToSet);
EventBits_t xEventGroupClearBits(EventGroupHandle_t xEventGroup, EventBits_t uxBitsToClear);
EventBits_t xEventGroupWaitBits(EventGroupHandle_t xEventGroup, EventBits_t uxBitsToWaitFor,
                                BaseType_t xClearOnExit, BaseType_t xWaitForAll, TickType_t xTicksToWait);
```

### Notes Event Group

- Useful when task needs to wait for multiple conditions (bits).
- Bits are defined as `1<<n` flags.

---

## 7. Software Timers

### Key APIs Software Timers

```c
TimerHandle_t xTimerCreate(const char * const pcTimerName, TickType_t xTimerPeriodInTicks,
                           UBaseType_t uxAutoReload, void * pvTimerID, TimerCallbackFunction_t pxCallbackFunction);
BaseType_t xTimerStart(TimerHandle_t xTimer, TickType_t xTicksToWait);
BaseType_t xTimerStop(TimerHandle_t xTimer, TickType_t xTicksToWait);
BaseType_t xTimerChangePeriod(TimerHandle_t xTimer, TickType_t xNewPeriod, TickType_t xTicksToWait);
```

### Notes Software Timers

- Timer callbacks run in the **timer service task**, not in ISR.
- Good for periodic work without creating a dedicated task.

---

## 8. Stream / Message Buffers (for byte streams)

### Key APIs Stream

```c
StreamBufferHandle_t xStreamBufferCreate(size_t xBufferSizeBytes, size_t xTriggerLevelBytes);
size_t xStreamBufferSend(StreamBufferHandle_t xBuffer, const void *pvTxData, size_t xDataLength,
                         TickType_t xTicksToWait);
size_t xStreamBufferReceive(StreamBufferHandle_t xBuffer, void *pvRxData, size_t xBufferLength,
                            TickType_t xTicksToWait);

// From ISR variants available
```

### Notes Stream

- Use for UART-like data streams between tasks or ISR→task.

---

## 9. FromISR rules — do's and don'ts

- Always use `...FromISR` variants inside ISRs (e.g., `xQueueSendFromISR`, `xSemaphoreGiveFromISR`, `xTaskNotifyGiveFromISR`).
- Declare `BaseType_t xHigherPriorityTaskWoken = pdFALSE;` and pass to FromISR functions; call `portYIELD_FROM_ISR(xHigherPriorityTaskWoken)` after to request context switch if needed.
- Use `xTaskGetTickCountFromISR()` if needing tick count inside ISR.
- Keep ISR **minimal**: set flags, give semaphore/notify, and return.

---

## 10. Static allocation (no heap) — safe production use

- Tasks:

```c
StaticTask_t xTaskBuffer;
StackType_t xStack[STACK_SIZE];
xTaskCreateStatic(vTask, "T", STACK_SIZE, NULL, priority, xStack, &xTaskBuffer);
```

- Semaphores / Queues:

```c
StaticSemaphore_t xSemBuf;
SemaphoreHandle_t sem = xSemaphoreCreateBinaryStatic(&xSemBuf);

StaticQueue_t xQueueBuffer;
uint8_t ucQueueStorageArea[QUEUE_BYTES];
QueueHandle_t q = xQueueCreateStatic(length, itemSize, ucQueueStorageArea, &xQueueBuffer);
```

- Enable `configSUPPORT_STATIC_ALLOCATION` in FreeRTOSConfig.h.

---

## 11. Diagnostics & Debugging

- `uxTaskGetStackHighWaterMark(handle)` — stack usage low water mark.
- `uxQueueMessagesWaiting(q)` — number of items waiting.
- `vTaskList(NULL)` (with trace facility) — task status table.
- Enable `configCHECK_FOR_STACK_OVERFLOW` and `configUSE_TRACE_FACILITY` for debugging.

---

## 12. Common patterns and pitfalls

- **Pattern: Producer-Consumer** → queue between ISR/producer and task/consumer.
- **Pattern: ISR → signal Task** → binary semaphore or task notification.
- **Pattern: Protect resource** → mutex between tasks (not ISR).
- **Pitfalls**:
  - Don’t call blocking API in ISR.
  - Don’t use mutex in ISR.
  - Avoid calling heavy functions inside ISR (`malloc`, `Serial.print`, `millis()` sometimes).
  - Always check `NULL` on create APIs.
  - When using `portMAX_DELAY`, ensure paired give will happen to avoid deadlock.

---

## 13. Short examples collection

### Producer / Consumer (Queue)

```c
QueueHandle_t q = xQueueCreate(10, sizeof(int));
void vProducer(void *p){ int i=0; for(;;){ xQueueSend(q, &i, pdMS_TO_TICKS(100)); i++; vTaskDelay(pdMS_TO_TICKS(200)); } }
void vConsumer(void *p){ int rx; for(;;){ if(xQueueReceive(q, &rx, portMAX_DELAY)==pdTRUE) Serial.println(rx); } }
```

### ISR -> Binary Semaphore -> Task

```c
SemaphoreHandle_t binSem = xSemaphoreCreateBinary();
void IRAM_ATTR isr(){ BaseType_t hpw=pdFALSE; xSemaphoreGiveFromISR(binSem, &hpw); portYIELD_FROM_ISR(hpw); }
void vHandler(void *p){ for(;;){ if(xSemaphoreTake(binSem, portMAX_DELAY)==pdTRUE) { /* handle */ } } }
```

### Mutex protecting shared counter

```c
SemaphoreHandle_t m = xSemaphoreCreateMutex();
int counter = 0;
void vInc(void *p){ for(;;){ if(xSemaphoreTake(m, portMAX_DELAY)==pdTRUE){ counter++; xSemaphoreGive(m); } vTaskDelay(100); } }
```

---

## References & further reading

- [FreeRTOS docs](https://www.freertos.org)  
- API reference and guides: search `FreeRTOS reference manual`, `FreeRTOS API`  
- ESP-IDF / Arduino-ESP32 FreeRTOS adaptations and `IRAM_ATTR` usage.

---

\*EOF — cheatsheet.md\*

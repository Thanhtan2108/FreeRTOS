# TẠO TASK TRONG FREERTOS

## xTaskCreate

```c
BaseType_t xTaskCreate( TaskFunction_t pvTaskCode,
                        const char * const pcName,
                        const uint32_t usStackDepth,
                        void *pvParameters,
                        UBaseType_t uxPriority,
                        TaskHandle_t *pxCreatedTask );
```

Giải thích tham số:

### TaskFunction_t pvTaskCode (Tên_Hàm_Xử_Lý_Task)

- Phải có kiểu: `void function_name(void *pvParameters)` — `trả về void` (không trả về giá trị!).

- `function_name` được đặt theo cú pháp : `v<NameFunction>Task`.

- Nếu `có tham số truyền vào` thì phải là tham số kiểu `void`, sau đó tiến hành `ép kiểu con trỏ` cho tham số.

- Hàm này thường gồm: khởi tạo (setup) + vòng lặp xử lý (loop). Nếu `task kết thúc thì có thể gọi vTaskDelete(NULL); hoặc không cần trước khi rời khỏi`, vì `hàm không được return`.

- Lưu ý: không trả về; nếu bạn vô tình return thì hành vi không xác định.

### const char * const pcName (Tên_Của_Task_Này)

- Tên dùng cho debug / trace. `Độ dài tối đa = configMAX_TASK_NAME_LEN (xác định trong FreeRTOSConfig.h)`.

- Tên không ảnh hưởng hành vi scheduler, chỉ để dễ đọc khi debug.

### const uint32_t usStackDepth (kích_thước_stack)

- Kích thước stack là số “đơn vị stack” (port-dependent). Trên nhiều `port` đơn vị này là `words` (ví dụ 32-bit words), không phải byte — hãy kiểm tra `port` của bạn (`xem portSTACK_TYPE / configSTACK_DEPTH_TYPE / tài liệu port`).

- Dùng `configMINIMAL_STACK_SIZE` làm mốc và tăng thêm theo nhu cầu (những hàm dùng nhiều biến cục bộ, chuỗi, printf sẽ cần nhiều stack).

- Nếu không chắc, `dùng uxTaskGetStackHighWaterMark()` hoặc `bật configCHECK_FOR_STACK_OVERFLOW` để kiểm tra `runtime`.

### *void pvParameters (Tham_số_truyền_vào_Task)

- Truyền con trỏ đến dữ liệu; dữ liệu phải tồn tại (không được truyền con trỏ tới biến cục bộ đã ra ngoài phạm vi). `Thường truyền địa chỉ` của `struct` hoặc `pointer đến queue/handle`.

- Nếu `không dùng` thì truyền `NULL`.

- Nếu task cần tham số nhỏ (ví dụ 32-bit), thường truyền pointer tới vùng tĩnh hoặc cast trực tiếp (nhưng cẩn thận với kích thước pointer vs integer).

### UBaseType_t uxPriority (mức_độ_ưu_tiên)

- Giá trị nguyên từ `0 đến configMAX_PRIORITIES - 1`. 0 = thấp nhất. Số lớn hơn => ưu tiên cao hơn.

- `Scheduler FreeRTOS mặc định là priority-based preemptive (nếu configUSE_PREEMPTION == 1)`.

- Quy tắc: chỉ dùng mức ưu tiên cao cho tác vụ thực sự cần. Không nên dùng quá nhiều mức khác nhau; tránh starvation.

### *TaskHandle_t pxCreatedTask (taskhandler_của_Task)

- Đây `là con trỏ` tới một `biến loại TaskHandle_t` mà API sẽ ghi vào nếu tạo task thành công.

- khởi tạo pxCreatedTask bằng `TaskHandle_t x<NameTask>Handle = NULL;`

- Nếu bạn cần `thao tác với task` sau này (xóa, treo, đổi ưu tiên, notify, v.v.), truyền `&myHandle`. Nếu `không cần`, truyền `NULL`.

- Ví dụ sử dụng:

```c
vTaskDelete(myHandle); 
vTaskSuspend(myHandle); 
vTaskPrioritySet(myHandle, newPrio);
xTaskNotifyGive(myHandle); 
xTaskGetHandle("TaskName");
// v.v.
```

- Lưu ý: nhiều API nhận NULL để chỉ task hiện tại (vd. vTaskDelete(NULL) xóa chính task gọi).

## Giá trị trả về của xTaskCreate

- Kiểu trả về: `BaseType_t`.

- Giá trị `thành công` thường là `pdPASS (non-zero)`. Nếu `không đủ bộ nhớ động => trả lỗi` (ví dụ `errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY hoặc pdFAIL`). Luôn kiểm tra trả về!

## xTaskCraetePinnedToCore() trong FreeRTOS (ESP32)

ESP32 là vi điều khiển 2 lõi (core 0 và core 1), nên Espressif mở rộng FreeRTOS bằng API đặc biệt cho phép bạn “ghim” (pin) task vào một lõi cụ thể, hoặc để scheduler tự chọn lõi.

📘 Cú pháp

```c
BaseType_t xTaskCreatePinnedToCore(
    TaskFunction_t pvTaskCode,
    const char * const pcName,
    const uint32_t usStackDepth,
    void * const pvParameters,
    UBaseType_t uxPriority,
    TaskHandle_t * const pvCreatedTask,
    const BaseType_t xCoreID
);
```

🔍 Giải thích từng tham số

| Tham số             | Mô tả chi tiết                                                                                                                                                                                                                  |
| ------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **`pvTaskCode`**    | Hàm xử lý task, dạng `void myTask(void *pvParameters)`. <br>Giống hoàn toàn với `xTaskCreate()`.                                                                                                                                |
| **`pcName`**        | Tên của task, chỉ dùng để debug/trace, giới hạn độ dài bởi `configMAX_TASK_NAME_LEN`.                                                                                                                                           |
| **`usStackDepth`**  | Kích thước stack của task (tính theo *words*, không phải bytes*). Dùng `configMINIMAL_STACK_SIZE` làm cơ sở.                                                                                                                    |
| **`pvParameters`**  | Con trỏ tham số truyền vào task, hoặc `NULL` nếu không dùng.                                                                                                                                                                    |
| **`uxPriority`**    | Mức ưu tiên task (`0` → thấp nhất). Task có priority cao hơn sẽ được ưu tiên hơn trong lịch trình.                                                                                                                              |
| **`pvCreatedTask`** | Con trỏ đến biến kiểu `TaskHandle_t` để API ghi vào nếu tạo thành công. <br> Dùng `NULL` nếu không cần giữ handle.                                                                                                              |
| **`xCoreID`**       | **Lõi CPU mà task sẽ chạy trên.** <br>Giá trị có thể là: <br>🧠 `0` → gán task chạy trên **Core 0** <br>⚙️ `1` → gán task chạy trên **Core 1** <br>🕹️ `tskNO_AFFINITY` → để scheduler tự chọn lõi (giống như `xTaskCreate()`). |

📤 Giá trị trả về

- `pdPASS` → tạo task thành công

- `errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY` hoặc `pdFAIL` → thất bại (thường do thiếu RAM heap)

💡 Lưu ý cực quan trọng với ESP32

| Thông tin                                                                                                                                                                   | Giải thích                                                                                                              |
| ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | ----------------------------------------------------------------------------------------------------------------------- |
| ESP32 có **2 lõi**:                                                                                                                                                           | - **Core 0**: thường xử lý Wi-Fi, Bluetooth, system task<br>- **Core 1**: thường dành cho user app (Arduino, main loop) |
| Nếu bạn dùng **Arduino framework**, thì **loop()** và các task mặc định chạy trên **Core 1**.                                                                                 |                                                                                                                         |
| Nên dùng `xTaskCreatePinnedToCore()` khi bạn muốn **phân tách rõ ràng** workload giữa 2 core (ví dụ: task điều khiển real-time chạy Core 0, task giao tiếp mạng chạy Core 1). |                                                                                                                         |
| Nếu không cần quan tâm, dùng `xTaskCreate()` là đủ (tự động chọn core).                                                                                                       |                                                                                                                         |

Nếu bạn dùng Arduino framework, thì loop() và các task mặc định chạy trên Core 1.

Nên dùng `xTaskCreatePinnedToCore()` khi bạn muốn phân tách rõ ràng workload giữa 2 core (ví dụ: task điều khiển real-time chạy Core 0, task giao tiếp mạng chạy Core 1).

Nếu không cần quan tâm, dùng `xTaskCreate()` là đủ (tự động chọn core).

## Delay trong FreeRTOS

- Trong FreeRTOS, không nên dùng hàm `delay(timeMS);` để delay vì nó sẽ kiến cho task bị treo, làm hệ thống bị treo theo => hệ thống không hoạt động tốt.

- Dùng `vTaskDelay(timeMS/portTICK_PERIOD_MS);` hoặc `vTaskDelay(pdMS_TO_TICKS(timeMS));` để thay thế cho `delay(timeMS);`.

- Khi gặp API `vTaskDelay(timeMS/portTICK_PERIOD_MS);`hoặc `vTaskDelay(pdMS_TO_TICKS(timeMS));` , task sẽ chuyển sang task mới và sau `timeMS` sẽ quay lại thực hiện task chứ không treo task.

# SEMAPHORE TRONG FREERTOS

## Semaphore

Việc tạo ra nhiều task trong FreeRTOS gọi là multitasking

Các task có thể giao tiếp với nhau (ví dụ: qua biến toàn cục), nhưng không nên giao tiếp trực tiếp nếu không có cơ chế đồng bộ. Để truyền/nhận dữ liệu an toàn giữa các task, cần dùng Queue. Để đồng bộ (signal, bảo vệ tài nguyên, đếm tài nguyên) thì dùng Semaphore.

### A. Semaphore là gì?

- Semaphore là 1 object của kernel dùng để đồng bộ, quản lý tài nguyên và bảo vệ tài nguyên của hệ thống.

- Semaphore hoạt động theo dạng token/flag: khi một task hoàn thành công việc nó give() (tăng token), task khác take() (giảm token) và thực hiện.

- Mức độ ưu tiên của các task có ảnh hưởng đến hệ thống. Ví dụ task có độ ưu tiên lớn hơn sẽ được thực thi và có thể nó chiếm dụng nhiều thời gian của hệ thống khiến các task có độ ưu tiên thấp không được thực thi. Trong khi các dữ liệu vẫn liên tục cần xử lý (chẳng hạn dữ liệu từ cảm biến gửi về task này). Điều này có thể gây ra mất dữ liệu cà ứng dụng sẽ hoạt động sai

-> Cần phải bảo vệ tài nguyên của hệ thống khởi sự mất mát này

-> Semaphore

- Semaphore là 1 cơ chế báo hiệu, trong đó 1 task ở trạng thái được chờ báo hiệu bởi 1 task khác để thực hiện. Semaphore sử dụng 1 key (hay flag - cờ), có thể là 1 số nguyên. Khi 1 task (Task1) hoàn thành công việc, nó sẽ tăng cờ này lên 1 đơn vị. Task2 sẽ nhận cờ này và được thực hiện công việc của mình. Khi Task2 hoàn thành công việc, cờ sẽ giảm đi 1 đơn vị.

### B. Các loại Semaphore

#### Binary Semaphore

- Có 2 giá trị là 0 và 1, khá giống 1 Queue có độ dài là 1.

- Ví dụ Task1 gửi dữ liệu qua Task2. Task2 sẽ kiểm tra item ở Queue, nếu thấy nó bằng 1 thì Task2 sẽ đọc dữ liệu về và giảm cờ xuống 0. Sau đó Task2 sẽ đợi tiếp đến khi cờ này bằng 1 để nhận dữ liệu.

-> Đồng bộ hóa dữ liệu gữa các task.

#### Counting Semaphore

- Có giá trị lớn hơn 0 (chẳng hạn từ 0 -> 255), gần giống 1 hàng đợi có độ dài lớn hơn 1. Sử dụng để đếm các sự kiện. Mỗi khi có 1 sự kiện xảy ra sẽ tăng giá trị đếm Semaphore. Mỗi Task xử lý 1 sự kiện sẽ lấy (giảm) đi 1 đơn vị đếm Semaphore.

-> Giá trị đếm là chênh lệch giữa số sự kiện xảy ra và số sự kiện đã xử lý.

### Sử dụng Semaphore

- FreeRTOS cung cấp các API khác nhau đeer create, give và take semaphore. Nếu dùng ngắt thì dùng các API được bảo vệ bởi ngắt.

#### Include thư viện

- Thư viện cho FreeRTOS

```c
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
```

- Thư viện sử dụng semaphore

```c
#include "freertos/semphr.h"
```

#### Handle Semaphore

- Tạo 1 handler cho semaphore (cơ chế tương tự như Queue)

```c
// khai báo handle (chưa tạo)
SemaphoreHandle_t xSemaphore = NULL;
```

Trong đó:

- `SemaphoreHandle_t` là kiểu handle opaque do FreeRTOS cung cấp (bên trong nó thường là `QueueHandle_t`).

- `xSemaphore` là tên biến (bạn đặt tuỳ ý).

- Khởi tạo `= NULL` là best practice.

#### Create Semaphore

Tạo ra semaphore binary trong FreeRTOS

```c
SemaphoreHandle_t xSemaphoreCreateBinary(void);
```

Tạo ra semaphore counting trong FreeRTOS

```c
SemaphoreHandle_t xSemaphoreCreateCounting(UBaseType_t uxMaxCount, UBaseType_t uxInitialCount);
```

Trong đó:

- `uxMaxCount` — số token tối đa.

- `uxInitialCount` — số token ban đầu (0...uxMaxCount).

Trả về: handle hoặc NULL.

#### Delete Semaphore

Xóa Semaphore

```c
void vSemaphoreDelete( SemaphoreHandle_t xSemaphore );
```

- Sau `vSemaphoreDelete()` nên gán `xSemaphore = NULL` để tránh dangling handle.

- Tránh xóa semaphore khi vẫn còn task đang chờ trên nó — hành vi không mong muốn.

#### Give Semaphore

Gửi 1 đơn vị Semaphore đến Semaphore (non-ISR)

```c
BaseType_t xSemaphoreGive( SemaphoreHandle_t xSemaphore );
```

- Dùng trong ngữ cảnh task (non-ISR).

- Mục đích: trả token (release) cho semaphore (binary hoặc counting). Dùng cho binary semaphore, counting semaphore, hoặc mutex (non-recursive).

- Tham số: `xSemaphore` — handle đã được tạo (không NULL).

- Trả về: `pdTRUE` (hoặc tương đương `pdPASS`) nếu thao tác thành công; `pdFALSE` nếu thất bại (ví dụ nếu `counting semaphore` đã đạt `uxMaxCount` hoặc handle không hợp lệ — tùy port).

- Blocking? Không — là thao tác ngay lập tức, không có timeout.

- Ghi chú: Với binary semaphore, nếu semaphore đang ở trạng thái AVAILABLE (1 token), gọi `xSemaphoreGive()` lần nữa thường không tăng trạng thái (vẫn chỉ 1 token) — tức gọi thêm có thể không có tác dụng (cách xử lý phụ thuộc phiên bản/implement).

Gửi 1 đơn vị Semaphore từ Semaphore có xử lý ngắt

```c
BaseType_t xSemaphoreGiveFromISR( SemaphoreHandle_t xSemaphore, BaseType_t *pxHigherPriorityTaskWoken );
```

- Dùng trong ngữ cảnh ISR (interrupt).

- Mục đích: trả token từ ISR cho binary/counting semaphore.

- Tham số:

    `xSemaphore` — handle semaphore.

    `pxHigherPriorityTaskWoken` — con trỏ tới biến `BaseType_t` (khởi tạo `pdFALSE`) sẽ được set `pdTRUE` nếu việc `Give` đã làm unblock một task có ưu tiên cao hơn task đang chạy.

- Trả về: `pdTRUE` nếu thao tác thành công; `pdFALSE` nếu thất bại.

- Sau ISR: nếu `*pxHigherPriorityTaskWoken == pdTRUE` → phải gọi `portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);` (hoặc `portEND_SWITCHING_ISR()` tuỳ port) để cho scheduler chuyển ngay sang task đã được unblock.

- Ghi chú: Không dùng `xSemaphoreGive()` trong ISR — dùng hàm `FromISR` này.

#### Take Semaphore

Lấy 1 đơn vị semaphore từ semaphore đã được gửi tới

```c
BaseType_t xSemaphoreTake( SemaphoreHandle_t xSemaphore, TickType_t xTicksToWait );
```

Trong đó:

- `xSemaphore` — handle của semaphore (kiểu `SemaphoreHandle_t`) đã được tạo bằng `xSemaphoreCreateBinary()`, `xSemaphoreCreateCounting()`, v.v.

- `xTicksToWait` — số tick muốn chờ nếu semaphore chưa có token / không thể lấy ngay. Là thời gian chờ tối đa mà Task rơi vào trạng thái Block để đợi đến khi sử dụng được Semaphore

    `0` → không chờ (non-blocking).

    `pdMS_TO_TICKS(ms)` → chuyển ms → ticks (thường dùng).

    `portMAX_DELAY` → chờ vô hạn (nếu `configINCLUDE_vTaskSuspend == 1`).

Giá trị trả về:

- Trả `pdTRUE` (hoặc `pdPASS`) nếu lấy thành công (semaphore có token).

- Trả `pdFALSE` nếu hết thời gian chờ (timeout) hoặc thao tác thất bại.

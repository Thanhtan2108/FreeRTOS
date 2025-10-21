# Giải thích 1 số điểm chú ý trong code

## 1️⃣ constexpr và volatile là gì? Khi nào cần dùng?

✅ `constexpr`

- Là từ khóa của C++, dùng để nói với trình biên dịch rằng giá trị này được biết ngay tại thời điểm biên dịch (compile-time constant).

- Giúp chương trình chạy nhanh hơn, an toàn hơn vì compiler có thể thay thế trực tiếp giá trị.

Ví dụ:

```cpp
constexpr uint32_t DEBOUNCE_TIME_MS = 200;
```

➡️ Compiler sẽ thay luôn 200 vào mã máy, không cần cấp phát RAM cho biến này.

Nếu bạn viết:

```cpp
long deboucingTime = 200;
```

thì biến đó nằm trong RAM, và CPU phải đọc nó lúc chạy → tốn tài nguyên hơn.

Dùng `constexpr` hoặc `const` khi:

- Biến không bao giờ thay đổi.

- Biến chỉ mang giá trị cấu hình (VD: thời gian delay, số chân GPIO, ngưỡng so sánh...).

⚡ `volatile`

- Là từ khóa nói với trình biên dịch rằng giá trị của biến này có thể thay đổi bất kỳ lúc nào, bên ngoài luồng điều khiển bình thường của chương trình (ví dụ bị thay đổi bởi ngắt ISR hoặc DMA, hoặc phần cứng).

Ví dụ:

```cpp
volatile bool buttonPressed = false;
```

➡️ Cho biết rằng biến này có thể bị thay đổi bởi ISR, vì ISR chạy ngoài luồng chính.

Nếu không có `volatile`, compiler có thể tối ưu sai, vì nó nghĩ biến này không thay đổi → dẫn đến lỗi logic rất khó debug.

Dùng `volatile` khi:

- Biến được thay đổi trong ISR.

- Biến được thay đổi bởi phần cứng (thanh ghi, cờ trạng thái, cổng I/O, ...).

- Biến được dùng trong đa luồng (multitasking) mà không có bảo vệ bằng semaphore/mutex.

## 2️⃣ Tại sao lại có 2 hàm IRAM_ATTR? Có phải là ngắt lồng nhau không?

Trong code bạn có 2 hàm có `IRAM_ATTR`:

```cpp
void IRAM_ATTR vButtonISR();                  // ISR thực tế được attachInterrupt() gọi
void IRAM_ATTR vGiveButtonSemaphoreFromISR(); // Hàm phụ trợ, được ISR gọi
```

👉 Đây không phải là 2 ngắt lồng nhau.
Mà là:

- `vButtonISR()` là hàm ngắt chính do phần cứng gọi.

- `vGiveButtonSemaphoreFromISR()` chỉ là hàm bình thường, được bạn gọi từ trong ISR.

⚙️ Lý do dùng `IRAM_ATTR` cho cả 2:

ESP32 chia bộ nhớ thành Flash (lưu code chính) và IRAM (RAM instruction).
Khi ngắt xảy ra, hệ thống có thể đang truy cập Flash, và nếu lúc đó code ISR nằm trong Flash → ESP32 có thể crash hoặc bị “Guru Meditation Error”.

Do đó, bất kỳ hàm nào được gọi từ ISR cũng nên nằm trong IRAM để:

- Đảm bảo an toàn khi ngắt.

- Không bị lỗi Flash busy.

Nên, gắn `IRAM_ATTR` cho cả 2 là chuẩn an toàn.

🔍 Tóm lại:

| Hàm                             | Vai trò                             | Có phải ISR thật không? | Vì sao có `IRAM_ATTR` |
| ------------------------------- | ----------------------------------- | ----------------------- | --------------------- |
| `vButtonISR()`                  | Hàm ISR chính, gọi khi có ngắt GPIO | ✅ Có                    | Bắt buộc              |
| `vGiveButtonSemaphoreFromISR()` | Hàm con được ISR gọi                | ❌ Không                 | Khuyến nghị (an toàn) |

### 3️⃣ Sao 1 hàm ISR có dùng millis(), 1 hàm thì không?

Cùng trong ISR, nhưng chỉ có `vButtonISR()` gọi `millis()`, còn `vGiveButtonSemaphoreFromISR()` thì không được phép gọi `millis()`.

⚠️ Nguyên nhân:

`millis()` dựa vào timer của hệ thống — cụ thể là một ngắt định kỳ (SysTick) để tăng bộ đếm mili-giây.

Nếu bạn gọi `millis()` từ bên trong ISR, thì:

- Bạn đang truy cập tài nguyên được dùng trong ngắt khác (SysTick).

- Dễ gây ra xung đột hoặc không chính xác, đặc biệt nếu ngắt của bạn có độ ưu tiên cao hơn timer ISR.

- Trên ESP32, đôi khi nó vẫn hoạt động, nhưng không khuyến khích (vì không thread-safe và không reentrant).

**Kết luận**: Vì debounce chỉ kiểm tra một vài lần nhấn (ít tần suất), nên việc dùng millis() trong ISR thường không sao trên ESP32 thực tế, và được Arduino core cho phép.

→ Dùng OK, nhưng phải biết đây không phải cách tốt nhất trong chuẩn FreeRTOS.

📘 Tóm tắt ngắn gọn

| Mục                  | Ý nghĩa                                          | Khi dùng                                                         |
| -------------------- | ------------------------------------------------ | ---------------------------------------------------------------- |
| `constexpr`          | Hằng số compile-time (tối ưu bộ nhớ)             | Khi giá trị **không đổi**                                        |
| `volatile`           | Báo compiler biến có thể bị thay đổi ngoài luồng | Khi dùng trong **ISR** hoặc **đa luồng**                         |
| `IRAM_ATTR`          | Lưu code trong RAM (IRAM) để ISR an toàn         | Cho **hàm ISR** và **hàm gọi từ ISR**                            |
| `millis()` trong ISR | Không khuyến khích (dễ lỗi)                      | Chỉ dùng nếu **thật đơn giản và ít tần suất**, hoặc tách ra task |

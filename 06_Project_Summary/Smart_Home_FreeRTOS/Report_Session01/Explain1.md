# Giải thích thêm các câu lệnh trong buổi 1

Giải thích giúp 2 lệnh này

```ini
build_flags = 
    -D CORE_DEBUG_LEVEL=1
    -Wl,-Map=output.map

monitor_filters = 
    log2file
    time
```

## 🧱 1. build_flags

Đây là các tham số (flags) được truyền cho trình biên dịch (gcc) hoặc trình liên kết (ld) khi build chương trình.

```ini
build_flags = 
    -D CORE_DEBUG_LEVEL=1
    -Wl,-Map=output.map
```

🔹 -D CORE_DEBUG_LEVEL=1

- -D có nghĩa là định nghĩa macro (define macro) — tương đương với dòng #define CORE_DEBUG_LEVEL 1 trong code.

- Ở đây, bạn gán CORE_DEBUG_LEVEL=1.

- Mức độ debug (CORE_DEBUG_LEVEL) thường được thư viện hoặc SDK (ví dụ ESP-IDF hoặc Arduino core) sử dụng để bật/tắt hoặc điều chỉnh độ chi tiết của log debug.

Ví dụ với ESP32:

| Mức | Ý nghĩa                     |
| --- | --------------------------- |
| 0   | Không log gì cả             |
| 1   | Chỉ log lỗi (ERROR)         |
| 2   | Log cảnh báo (WARNING)      |
| 3   | Log thông tin (INFO)        |
| 4   | Log debug (DEBUG)           |
| 5   | Log chi tiết nhất (VERBOSE) |

👉 Nghĩa là: CORE_DEBUG_LEVEL=1 chỉ hiển thị log lỗi (Error).

🔹 -Wl,-Map=output.map

- -Wl,option nghĩa là “gửi option này đến linker” (ld), thay vì compiler.

- -Map=output.map bảo linker tạo file bản đồ (map file) có tên output.map.

Tác dụng của file .map:

- Là file chứa thông tin chi tiết về cách chương trình được đặt trong bộ nhớ:

  - địa chỉ từng hàm, biến toàn cục, section .text, .data, .bss...

  - kích thước từng phần.

- Rất hữu ích để phân tích dung lượng flash/RAM và tối ưu bộ nhớ trong lập trình nhúng.

📁 Sau khi build xong, bạn sẽ thấy file output.map nằm trong thư mục build, ví dụ:

```bash
.pio/build/esp32doit-devkit-v1/output.map
```

## 🧩 2. monitor_filters

Đây là phần cấu hình cho serial monitor của PlatformIO (cửa sổ xem log UART khi chạy chương trình).

```ini
monitor_filters = 
    log2file
    time
```

🔹 log2file

- Tự động ghi toàn bộ log UART vào file trong thư mục .pio/build/.../monitor.log.

- Giúp bạn lưu lại lịch sử log để phân tích sau (đặc biệt hữu ích khi test dài hoặc debug crash).

Ví dụ log được lưu thành:

```bash
.pio/build/esp32doit-devkit-v1/monitor.log
```

🔹 time

- Thêm dấu thời gian (timestamp) trước mỗi dòng log khi hiển thị trên monitor.

- Giúp bạn biết thời điểm từng log xuất hiện → rất hữu ích để đo độ trễ, thời gian phản hồi, v.v.

Ví dụ hiển thị trên terminal:

```makefile
00:00:01.234 -> System initialized
00:00:03.567 -> Sensor value: 25.6°C
```

## ✅ Tóm lại

| Lệnh                    | Tác dụng                                       |
| ----------------------- | ---------------------------------------------- |
| `-D CORE_DEBUG_LEVEL=1` | Đặt mức log debug (1 = chỉ log lỗi)            |
| `-Wl,-Map=output.map`   | Tạo file `.map` chứa sơ đồ bộ nhớ sau khi link |
| `log2file`              | Lưu toàn bộ log UART vào file                  |
| `time`                  | Hiển thị thời gian trước mỗi dòng log          |

Nếu bạn muốn:

- bật nhiều log hơn → dùng CORE_DEBUG_LEVEL=3 hoặc 4

- lưu log ra file theo tên tùy ý → mình có thể chỉ bạn cách tùy chỉnh lại monitor_logpath.

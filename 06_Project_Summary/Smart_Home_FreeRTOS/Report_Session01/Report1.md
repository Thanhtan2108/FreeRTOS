# Báo Cáo Tuần 1

##

### ✅ MODULES HOẠT ĐỘNG TỐT

✅ : Nếu module hoạt động tốt

❌ : Nếu module không hoạt tốt, có lỗi

- [✅] OLED Display

  - ✅ Màn hình sáng lên

  - ✅ Hiển thị rõ ràng các dòng chữ

  - ✅ Không bị nháy hay mất hình

- [✅] DS18B20 Temperature

  - ✅ Tìm thấy ít nhất 1 device

  - ✅ Nhiệt độ trong khoảng 15-35°C (nhiệt độ phòng)

  - ✅ Giá trị ổn định, không nhảy loạn

- [✅] DHT11 Temperature/Humidity

  - ✅ Nhiệt độ: 15-35°C

  - ✅ Độ ẩm: 30-80%

  - ✅ Giá trị ổn định sau vài lần đọc

- [✅] BH1750 Light Sensor

  - ✅ Giá trị lux >= 0

  - ✅ Thay đổi khi che/chiếu sáng cảm biến

  - ✅ Trong nhà: 100-500 lux, ngoài trời: 1000+ lux

- [❌] MQ135 Air Quality

  - ✅ Giá trị ppm >= 0

  - ✅ Thay đổi khi thổi hơi vào cảm biến

  - ✅ Ổn định dần sau vài phút

  - 🔧 Có `YÊU CẦU HỖ TRỢ` cho module MQ135 này

- [❌] Servo Motor

  - ❌ Không quay

- [❌] Relay

  - ❌ Không click

  - ✅ LED xanh lá trên module relay sáng

  - ❌ Không điều khiển được → Kiểm tra jumper cap trên relay thì đang kết nối kích relay mức HIGH.

### 🔧 YÊU CẦU HỖ TRỢ

- [🔧] Cần check code [moulde mq135_driver]
  
  - Em thực hiện cắm điện áp VCC = 3.3V cho MQ135

  - Em thực hiện đo cảm biến trong phòng, không khí không có khói, buội, chất độc hại, ít người,...

  - Nhưng khi đo cho kết quả quá bất ngờ vì giá trị quá cao, cao đến sai luôn

  ```text
  16:37:35.218 > 📊 Reading 1: 14780 ppm    
  16:37:35.218 > 🌬️  Chất lượng: Kém        
  16:37:37.218 > 📊 Reading 2: 15068 ppm    
  16:37:37.218 > 🌬️  Chất lượng: Kém        
  16:37:39.218 > 📊 Reading 3: 13754 ppm    
  16:37:39.218 > 🌬️  Chất lượng: Kém        
  16:37:41.217 > 📊 Reading 4: 8425 ppm     
  16:37:41.217 > 🌬️  Chất lượng: Kém        
  16:37:43.218 > 📊 Reading 5: 11475 ppm    
  16:37:43.218 > 🌬️  Chất lượng: Kém  
  ```

  - Em có thử tra cứu thì được biết ESP32 có độ phân giải 12 bit nên giá trị analog đọc được trong khoảng 0-4095.

  - Vì em làm project nhỏ thôi mà cần chờ MQ135 cần 24–48 giờ “warm-up” để ổn định lớp sưởi (heater) thì quá là không ổn vì em đâu thể đủ thời gian để test được.

  - Bên cạnh đó em còn tra cứu được là RZERO 76.63 là giá trị mặc định theo datasheet. Nếu chưa hiệu chuẩn trong không khí sạch, thì mọi giá trị ppm sẽ bị lệch. Nên đo Rs trong môi trường sạch (ngoài trời, không khí trong lành) và tính lại RZERO

- [🔧] Cần check code [moulde servo_driver]

  - em đổi chân cắm điều khiển servo sang chân GPIO 25 chứ không dùng chân GPIO 18 như ban đầu.

  - Em đã sửa lại cấu hình chân cho servo là 25 trong pin_config.h.

  - Nguồn ngõ vào em cấp từ pin 9V qua mạch buck LM2596 để điện áp ngõ ra là 5V cấp cho servo.

  - Khi nạp code thì Servo không quay mà chỉ đứng im

- [🔧] Cần check code [moulde relay_driver]

  - em đổi chân cắm điều khiển relay sang chân GPIO 13 chứ không dùng chân GPIO 23 như ban đầu.

  - Em đã sửa lại cấu hình chân cho relay là 13 trong pin_config.h.

  - Nguồn ngõ vào em cấp từ pin 9V qua mạch buck LM2596 để điện áp ngõ ra là 5V cấp cho relay.

  - Khi nạp code thì relay không bật, tắt hay phát ra ra tiếng click gì hết.

## ĐÃ FIX THÀNH CÔNG VÀ CÁC MODULE CHẠY TEST THÀNH CÔNG HẾT

Các việc làm để fix các module như sau:

- Thay vì dùng 1 viên pin 9V 1A cấp nguồn qua buck thì thay bằng 2 pin 18650 nối tiếp với nhau có điện áp khoảng 8.4VDC và dòng khoảng 4A, sau đó qua mạch buck để lấy được nguồn 5VDC cấp cho các thiết bị servo, relay.

- Với module servo, em thực hiện kết nối với GPIO 18

  - Sử dụng nguồn 5VDC từ ngõ ra của nguồn cấp của mạch buck LM2956 để cấp nguồn

  - Nối chân GND của servo với OUT- của LM2956 và GND của ESP32

- Với module relay, em thực hiện kết nối với GPIO 13
  
  - Sử dụng nguồn 5VDC từ ngõ ra của nguồn cấp của mạch buck LM2956 để cấp nguồn

  - Nối chân GND của relay với OUT- của LM2956 và GND của ESP32

  - Relay kích mức HIGH, cần kiểm tra chốt cắm quy định kích mức HIGH/LOW trên module

  **[Realy được kết nối với thiết bị như hình để thực hiện test](../image/connectRelay.png)**

  - Khi chân IN trên relay mức thấp, chân COM và NO nối với nhau -> Mạch hở

  - Khi chân IN trên relay mức cao, chân COM và NC nối với nhau -> Mạch đóng

- Với module mq135, em đã hiệu chỉnh lại thông số cho RZERO là 220.0
  
  - Hiệu chỉnh các thông số và có điều chỉnh lại nội dung kiến trúc hàm cho phù hợp
  
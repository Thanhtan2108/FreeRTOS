#ifndef DHT11_DRIVER_H
#define DHT11_DRIVER_H

#include <DHT.h>
#include "../../config/pin_config.h"  // Đã cập nhật

class DHT11Driver {
public:
    DHT11Driver();
    bool begin();
    float readTemperature();
    float readHumidity();
    bool isConnected();
    
private:
    DHT dht;
    bool initialized;
};

#endif

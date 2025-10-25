#ifndef BH1750_DRIVER_H
#define BH1750_DRIVER_H

#include <Wire.h>
#include <BH1750.h>
#include "../../config/pin_config.h"

class BH1750Driver {
public:
    BH1750Driver();
    bool begin();
    float readLightLevel();
    bool isConnected();
    
private:
    BH1750 lightMeter;
    bool initialized;
};

#endif

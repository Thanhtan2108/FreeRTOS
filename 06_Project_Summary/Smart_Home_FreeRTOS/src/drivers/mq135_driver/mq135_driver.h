#ifndef MQ135_DRIVER_H
#define MQ135_DRIVER_H

#include "../../config/pin_config.h"

class MQ135Driver {
public:
    MQ135Driver();
    bool begin();
    float readAirQuality();
    bool isConnected();
    float getCorrectedPPM(float temperature, float humidity);
    
private:
    bool initialized;
    float getResistance();
    float getCorrectedResistance(float temperature, float humidity);
};

#endif

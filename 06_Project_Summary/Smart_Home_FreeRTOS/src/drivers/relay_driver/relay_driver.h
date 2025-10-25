#ifndef RELAY_DRIVER_H
#define RELAY_DRIVER_H

#include "../../config/pin_config.h"

class RelayDriver {
public:
    RelayDriver();
    bool begin();
    void setState(bool state);
    void toggle();
    bool getState();
    bool isConnected();
    
private:
    bool initialized;
    bool currentState;
};

#endif

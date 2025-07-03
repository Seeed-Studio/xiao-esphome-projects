/* Class for a simple Zigbee On/Off Switch endpoint, inherited from common EP class */

#pragma once

#include "soc/soc_caps.h"
#include "sdkconfig.h"
#if CONFIG_ZB_ENABLED

#include "ZigbeeBinarySensor.h"
#include "ZigbeeEP.h"
#include "ha/esp_zigbee_ha_standard.h"

class ZigbeeHASwitch : public ZigbeeBinarySensor {
public:
    ZigbeeHASwitch(uint8_t endpoint_ids);
    ~ZigbeeHASwitch() {}

    // Methods to control the switch state and report it
    void toggle();
    void turnOn();
    void turnOff();

private:
    bool _switchState; // Current state of the switch (true = on, false = off)
};

#endif // CONFIG_ZB_ENABLED
#include "ZigbeeHASwitch.h"
#if CONFIG_ZB_ENABLED

ZigbeeHASwitch::ZigbeeHASwitch(uint8_t endpoint) : ZigbeeBinarySensor(endpoint)
{
    _switchState = false;
}

void ZigbeeHASwitch::toggle()
{
    _switchState = !_switchState;
    uint8_t state = _switchState ? 0x01 : 0x00;
    setStatus(state);
}

void ZigbeeHASwitch::turnOn()
{
    if (!_switchState)
    {
        _switchState = true;
        uint8_t on = 0x01;
        setStatus(on);
    }
}

void ZigbeeHASwitch::turnOff()
{
    if (_switchState)
    {
        _switchState = false;
        uint8_t off = 0x00;
        setStatus(off);
    }
}

#endif // CONFIG_ZB_ENABLED
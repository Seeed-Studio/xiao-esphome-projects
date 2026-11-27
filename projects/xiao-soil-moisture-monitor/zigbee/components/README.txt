Native Zigbee soil-status light

zigbee/ is a local snapshot of ESPHome 2026.11.0's zigbee component from the
installed official ESPHome package. Only zigbee_helpers_esp32.c is extended:
Power Configuration (0x0001), Level (0x0008), and Color Control (0x0300)
descriptor creation/add-attribute
routing delegate to Espressif esp-zigbee-lib 2.0.5's native SDK functions.
The dependency is intentionally pinned; review this patch before upgrading.
Source: https://github.com/esphome/esphome/tree/2026.11.0/esphome/components/zigbee
SDK: https://github.com/espressif/esp-zigbee-sdk

soil_status_light/ registers endpoint 3, HA profile 0x0104, device type 0x0102
(Color Dimmable Light), OnOff 0x0006, Level 0x0008, ColorControl 0x0300.
It reports on=true, level=254 and XY color from the measured soil state.
Capabilities=0x0008 (XY), color mode=1. Local SDK attributes remain readable.
Status reports use paced standard
ZCL Report Attributes frames over APS, bundling XY and color mode in one frame.
Enqueue failures retry up to three times; a second color frame follows.
Local enqueue success does not establish coordinator receipt. No Wi-Fi/API/MQTT added.

This is a logical HA color indicator, not the physical LEDs. It stays logically
on to preserve the color in HA while GPIO LEDs turn off for power savings.
Standard HA light controls may issue commands while the device is asleep;
they are not physical LED controls. Each measurement reasserts the true status.

No HA helper, custom quirk or additional app is needed. ZHA 2.4.3 needs
the documented update_entity automation to read XY after each moisture change.
A 100 ms parent poll receive window ends 750 ms after an XY read request
(minimum 1 second), with a 6 second fallback. Requests are observed, not consumed;
the SDK responds normally. This is not an HA state-update acknowledgement.
A fresh ZHA interview is needed when adding endpoint 3 or the battery cluster
to a cached device signature.

soil_battery/ adds Power Configuration to endpoint 1. GPIO0 is sampled once
per measurement (8 ADC samples). The voltage estimate maps 1.2V to 0% and
the configurable full voltage to 100%, clamped outside this range. BatteryVoltage uses 100mV units;
BatteryPercentageRemaining uses 0.5% units. AA nominal voltage remains 1.5V;
the full threshold is a voltage-estimation setting, not the rated voltage.
Unknown values use 0xff. Battery reports bundle both values and retry local
enqueue failure at most three times. HA battery discovery was observed; ADC
accuracy and battery current/lifetime have not been measured.

Endpoint 4 uses standard Analog Output for Battery Full Voltage: 1300-2000 mV,
step 10 mV, default 1600 mV. Integer units avoid float32 decimal tails in HA.
The template number restores NVS and boot code migrates legacy 1.3-2.0 V values.
HA discovery/display is verified; power-cycle persistence/migration is not.

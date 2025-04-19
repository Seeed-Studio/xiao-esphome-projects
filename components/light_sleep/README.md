# ESPHome Light Sleep Component

This is a custom ESPHome component that enables light sleep mode on ESP32 devices, allowing wake-up via a specified GPIO pin. It supports triggering actions upon wake-up, such as turning on a light, enabling WiFi, or logging messages.

## Features

- **Light Sleep Mode**: Puts the ESP32 into light sleep to save power, maintaining GPIO wake-up capability.
- **GPIO Wake-Up**: Configurable GPIO pin to wake the device from light sleep.
On-Wakeup Actions: Execute custom actions (e.g., turn on LED, enable WiFi) when the device wakes up.
- **Custom Action**: Provides a `light_sleep.enter` action to programmatically enter light sleep.

## Installation

Place the component files in your ESPHome configuration directory under `/config/esphome/custom_components/light_sleep/`:

- `light_sleep.h`
- `light_sleep.cpp`
- `__init__.py`
Reference the component in your YAML configuration using `external_components`.

## Configuration

### YAML Configuration Example

Below is an example configuration for a device that enters light sleep when a button is held for 9 seconds and performs actions upon wake-up.

```yaml
external_components:
  - source:
      type: local
      path: /config/esphome/custom_components/light_sleep

esphome:
  name: iot-button

api:
wifi:
  power_save_mode: LIGHT

light_sleep:
  id: sleep
  wakeup_pin: 9
  on_prepare_sleep: # do something before sleep
    - light.turn_on: led
    - delay: 0.5s
    - light.turn_off: led
  on_wakeup: # do something after wake up
    - light.turn_on: led
    - logger.log: "Woke up from light sleep"

output:
  - platform: gpio
    pin: GPIO2
    id: led_output

light:
  - platform: binary
    name: "LED"
    output: led_output
    id: led

binary_sensor:
  - platform: gpio
    pin: GPIO9
    name: "Button"
    on_press:
      - light_sleep.enter: 
        id: sleep

interval:
  - interval: 60s
    then:
      - light_sleep.enter:
        id: sleep
```

### Configuration Options

- **id** (Required): Unique identifier for the light sleep component (e.g., `light_sleep_1`).
- **wakeup_pin** (Required): GPIO pin number (0-22 for ESP32-C6) used to wake the device from light sleep.
- **on_wakeup** (Optional): List of actions to execute when the device wakes up. Supports standard ESPHome actions like `light.turn_on`, `wifi.enable`, or `logger.log`.
- **light_sleep.enter** (Action): Action to enter light sleep mode, referencing the component's ID.

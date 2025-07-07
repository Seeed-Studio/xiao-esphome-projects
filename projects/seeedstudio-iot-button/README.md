# Seeed Studio IoT Button

This project provides configurations and code for the Seeed Studio IoT Button, a versatile smart device based on the ESP32-C6. It supports both the first generation (V1) and second generation (V2) of the device, with options for ESPHome (WiFi) and Arduino with Zigbee programming.

**Note**: This repository contains ESPHome YAML configurations (`seeedstudio-iot-button.yaml` for V1, `seeedstudio-iot-button-v2.yaml` for V2) and Arduino code for both V1 and V2. Select the appropriate files based on your device version and programming preference.

## Features

### Common Features

- Multiple button actions: single, double, and long press.
- RGB LED feedback with customizable effects.
- Power-saving modes to extend battery life.
- Seamless integration with smart home systems.

### First Generation (V1)

- **ESPHome**:
  - WiFi connectivity with fallback AP mode.
  - Blue LED for status.
  - Light sleep mode after 2 minutes of inactivity.
  - *Updates*: Long-press sleep function removed, button sensitivity optimized.
- **Arduino with Zigbee**:
  - Zigbee communication.
  - Blue LED for status.
  - Light sleep mode after 2 minutes of inactivity.

### Second Generation (V2)

- **ESPHome**:
  - WiFi connectivity with fallback AP mode.
  - Battery monitoring (voltage and percentage).
  - Red and blue LEDs for status.
  - Light sleep mode after 2 minutes of inactivity, wakeup on button press.
- **Arduino with Zigbee**:
  - Zigbee communication.
  - Battery monitoring.
  - Red and blue LEDs for status.
  - Deep sleep mode after 2 minutes of inactivity, wakeup on button press.

## Hardware Pinout

### V1

- **User Button**: GPIO9
- **Blue User LED**: GPIO2 (inverted)
- **RGB Status LED**: GPIO19 (WS2812)
- **LED Strip Power Enable**: GPIO18

### V2

- **User Button**: GPIO2
- **Blue LED**: GPIO3
- **Red LED**: GPIO14
- **RGB Status LED**: GPIO19 (WS2812)
- **Battery Enable Pin**: GPIO0
- **Battery ADC Pin**: GPIO1

## Programming Options

### ESPHome

- **Overview**: Easy integration with Home Assistant via WiFi, configurable via YAML files, supports OTA updates.
- **Button Actions**:
  - Single press: Toggles Switch 1.
  - Double press: Toggles Switch 2.
  - Long press (1-2s): Toggles Switch 3.

### Arduino with Zigbee

- **Overview**: Uses Zigbee for communication, suitable for Zigbee networks, code available for both V1 and V2.
- **Button Events**:
  - Press/Release: Sends binary input.
  - Single click: Toggles Switch 1.
  - Double click: Toggles Switch 2.
  - Triple click: Ignored.
  - Short long press (1-5s): Toggles Switch 3.
  - Long press (>5s): Resets Zigbee.

## Usage

### ESPHome

- **Button Actions**:
  - *Single Press*: Toggles Switch 1, RGB "Blink" effect.
  - *Double Press*: Toggles Switch 2, RGB "Subtle Flicker" effect.
  - *Long Press (1-2s)*: Toggles Switch 3, RGB "Rainbow" effect.
- **Status LEDs**:
  - V1: Blue LED indicates status.
  - V2: Blue LED for normal operation, red LED for low battery/errors.
- **Power Saving**: Light sleep after 2 minutes of inactivity.

### Arduino with Zigbee

- **Button Events**:
  - *Press/Release*: Updates Zigbee binary input.
  - *Single Click*: Toggles Switch 1, breathing LED effect.
  - *Double Click*: Toggles Switch 2, blinking LED effect.
  - *Short Long Press (1-5s)*: Toggles Switch 3, rainbow LED effect.
  - *Long Press (>5s)*: Resets Zigbee.
- **Status LEDs**:
  - V1: Blue LED indicates Zigbee status.
  - V2: Blue LED for operation, red LED for low battery/disconnection.
- **Power Saving**:
  - V1: Light sleep after 2 minutes.
  - V2: Deep sleep after 2 minutes.

## Getting Started

### ESPHome

1. **Install ESPHome**: Follow the [ESPHome installation guide](https://esphome.io/guides/installing_esphome.html).
2. **Choose YAML**:
   - V1: `seeedstudio-iot-button.yaml`
   - V2: `seeedstudio-iot-button-v2.yaml`
3. **Flash the Device**:

   ```bash
   esphome run <your_yaml_file>.yaml
   ```

4. **Add to Home Assistant**: Discoverable via ESPHome integration.

### Arduino with Zigbee

1. **Install Arduino IDE**: Download from [Arduino.cc](https://www.arduino.cc/en/software).
2. **Update ESP32-SDK**: Make sure your version is 3.2.1 or above.
3. **Install Libraries**: FastLED.
4. **Load Code**:
   - V1: Use provided V1 Arduino code.
   - V2: Use provided V2 Arduino code.
5. **Configure and Upload**: Set Zigbee parameters, upload to device.

## Customization

- **ESPHome**: Edit YAML to adjust pins, actions, LED effects, or sleep settings.
- **Arduino with Zigbee**: Modify code for button mappings, LED behaviors, or Zigbee settings.

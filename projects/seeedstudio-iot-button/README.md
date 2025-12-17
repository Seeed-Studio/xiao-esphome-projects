# Seeed Studio IoT Button

This project provides configurations and code for the Seeed Studio IoT Button, a versatile smart device based on the ESP32-C6. It supports both the first generation (V1) and second generation (V2) of the device, with options for ESPHome (WiFi) and Arduino with Zigbee programming.

**Note**: This repository contains ESPHome YAML configurations (`seeedstudio-iot-button.yaml` for IoT Button V1, `seeedstudio-iot-button-v2.yaml` for IoT Button V2), **MQTT configuration (`seeedstudio-iotbutton_MQTT.yaml` for IoT Button V1)**, and Arduino code for both IoT Button V1 and IoT Button V2. Select the appropriate files based on your device version and programming preference.

## Features

### Common Features

- Multiple button actions: single, double, and long press.
- RGB LED feedback with customizable effects.
- Power-saving modes to extend battery life.
- Seamless integration with smart home systems.

### IoT Button V1

- **ESPHome**:
  - WiFi connectivity with fallback AP mode.
  - Blue LED for status.
  - Light sleep mode after 2 minutes of inactivity.
  - *Updates*: Long-press sleep function removed, button sensitivity optimized.
- **ESPHome (MQTT)**:
  - WiFi connectivity with MQTT broker support.
  - Blue LED for status.
  - RGB LED effects for button actions.
  - Light sleep mode after 2 minutes of inactivity.
  - Button actions and states published via MQTT topics.
- **Arduino with Zigbee**:
  - Zigbee communication.
  - Blue LED for status.
  - Light sleep mode after 2 minutes of inactivity.

### IoT Button V2

- **ESPHome**:
  - WiFi connectivity with fallback AP mode.
  - Battery monitoring (voltage and percentage).
  - Red and blue LEDs for status.
  - Deep sleep with wakeup on button press:
    - In provisioning (fallback AP) mode: enter deep sleep after 2 minutes of inactivity.
    - After connecting to WiFi: enter deep sleep after 30 seconds of inactivity.
- **Arduino with Zigbee**:
  - Zigbee communication.
  - Battery monitoring.
  - Red and blue LEDs for status.
  - Deep sleep mode after 2 minutes of inactivity, wakeup on button press.

## Hardware Pinout

### IoT Button V1

- **User Button**: GPIO9
- **Blue User LED**: GPIO2 (inverted)
- **RGB Status LED**: GPIO19 (WS2812)
- **LED Strip Power Enable**: GPIO18

### IoT Button V2

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

### ESPHome (MQTT)

- **Overview**: Integrates with any MQTT broker, suitable for non-Home Assistant setups.
- **Button Actions**:
  - Single press: Publishes ON/OFF to `button/switch_1/state` with RGB "Blink" effect.
  - Double press: Publishes ON/OFF to `button/switch_2/state` with RGB "Subtle Flicker" effect.
  - Long press (1-2s): Publishes ON/OFF to `button/switch_3/state` with RGB "Rainbow" effect.
- **Status LEDs**:
  - Blue LED indicates status.
- **Power Saving**: Light sleep after 2 minutes of inactivity.
- **Configuration**: See `seeedstudio-iotbutton_MQTT.yaml`.

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
- **Power Saving**:
  - V1: Light sleep after 2 minutes of inactivity.
  - V2: Deep sleep with wakeup on button press:
    - In provisioning (fallback AP) mode: enter deep sleep after 2 minutes of inactivity.
    - After connecting to WiFi: enter deep sleep after 30 seconds of inactivity.

### ESPHome (MQTT)

- **Button Actions**:
  - *Single Press*: Publishes ON/OFF to `button/switch_1/state`, RGB "Blink" effect.
  - *Double Press*: Publishes ON/OFF to `button/switch_2/state`, RGB "Subtle Flicker" effect.
  - *Long Press (1-2s)*: Publishes ON/OFF to `button/switch_3/state`, RGB "Rainbow" effect.
- **Status LEDs**:
  - Blue LED indicates status.
- **Power Saving**: Light sleep after 2 minutes of inactivity.
- **How to Use**:
  1. Configure `seeedstudio-iotbutton_MQTT.yaml` with your WiFi and MQTT broker information.
  2. Flash the firmware using the ESPHome tool.
  3. Button events will be published via MQTT topics (such as `button/switch_1/state`) and can be integrated into any MQTT platform.

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

#### **Recommended: Install Precompiled Firmware Easily**

- Visit [gadgets.seeed.cc](https://gadgets.seeed.cc/) to quickly install the latest precompiled firmware for your IoT Button. This is the fastest and easiest way to get started—no compilation required.

#### **Advanced: Compile and Flash Yourself**

1. **Install ESPHome**: Follow the [ESPHome installation guide](https://esphome.io/guides/installing_esphome.html).
2. **Choose YAML**:
    - IoT Button V1: `seeedstudio-iot-button.yaml`
    - IoT Button V2: `seeedstudio-iot-button-v2.yaml`
3. **Flash the Device**:

    ```bash
    esphome run <your_yaml_file>.yaml
    ```

4. **Add to Home Assistant**: Discoverable via ESPHome integration.

### ESPHome (MQTT)

1. **Install ESPHome**: Refer to the [ESPHome installation guide](https://esphome.io/guides/installing_esphome.html).
2. **Configure YAML**: Use `seeedstudio-iotbutton_MQTT.yaml` and fill in your WiFi and MQTT broker information.
3. **Flash the device**:

    ```bash
    esphome run seeedstudio-iotbutton_MQTT.yaml
    ```

4. **MQTT Integration**: Button events will be automatically published to your MQTT broker and can be subscribed to by any MQTT-compatible platform.

### Arduino with Zigbee

1. **Install Arduino IDE**: Download from [Arduino.cc](https://www.arduino.cc/en/software).
2. **Update ESP32-SDK**: Make sure your version is 3.2.1 or above.
3. **Install Libraries**: FastLED.
4. **Load Code**:
    - Use the provided Arduino code. Select your device version by defining either `IOT_BUTTON_V1` or `IOT_BUTTON_V2` in the code or build settings.
5. **Configure and Upload**:
    - Select the correct board: Tools → Board → ESP32 Arduino → XIAO_ESP32C6
    - Enable Zigbee Mode: Tools → Zigbee mode → Zigbee Endpoint
    - Set Partition Scheme: Tools → Partition Scheme → Zigbee 4MB with spiffs
    - For other settings, you can use the defaults
    - Select COM to Upload

## Customization

- **ESPHome**: Edit YAML to adjust actions, LED effects, or sleep settings.
- **ESPHome (MQTT)**: Edit `seeedstudio-iotbutton_MQTT.yaml` to customize button mapping, LED effects, MQTT topics, etc.
- **Arduino with Zigbee**: Modify code for button mappings, LED behaviors, or Zigbee settings.

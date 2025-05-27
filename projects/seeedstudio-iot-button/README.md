# Seeed Studio IoT Button

This project provides an ESPHome configuration for the Seeed Studio IoT Button, a versatile smart device based on the ESP32-C6. It supports WiFi connectivity, multiple button actions, RGB LED feedback, and deep/light sleep for low-power operation, making it ideal for smart home automation and Home Assistant integration.

## Features

- **WiFi Connectivity**: Connects to your WiFi network and supports fallback AP mode for easy setup.
- **Multiple Button Actions**: Single, double, and long press actions for flexible control.
- **RGB Status LED**: Visual feedback using a WS2812 RGB LED with multiple effects.
- **Blue User LED**: Indicates device status and actions.
- **Low Power Operation**: Supports light sleep mode, with automatic sleep on inactivity or long press.
- **Home Assistant Integration**: Seamless integration using the ESPHome API.
- **OTA Updates**: Supports over-the-air firmware updates.

## Hardware Pinout

- **User Button**: GPIO9
- **Blue User LED**: GPIO2 (inverted)
- **RGB Status LED**: GPIO19 (WS2812)
- **LED Strip Power Enable**: GPIO18

## Usage

- **Single Short Press**: Triggers Switch 1 (virtual toggle, customizable action).
- **Double Click**: Triggers Switch 2 (virtual toggle, customizable action).
- **Long Press (1-1.5s)**: Triggers Switch 3 (virtual toggle, customizable action).
- **Long Press (5s+)**: Enters light sleep mode for power saving.
- **Automatic Sleep**: Device enters light sleep after 2 minutes of inactivity.

## Getting Started

1. **Install ESPHome**:
   Follow the [ESPHome installation guide](https://esphome.io/guides/installing_esphome.html).

2. **Flash the Device**:
   Use the following command to upload the configuration:
   ```bash
   esphome run seeedstudio-iot-button.yaml
   ```

3. **Add to Home Assistant**:
   The device will be discoverable via the ESPHome integration.

## Customization

You can modify the YAML file to:
- Change pin assignments.
- Add or modify actions for single, double, or long press.
- Adjust LED effects or sleep behavior.
- Integrate with other smart home automations.

## 2、Purchase links and wiki

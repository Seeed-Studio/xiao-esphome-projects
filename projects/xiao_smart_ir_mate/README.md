# XIAO Smart IR Mate

This project is an ESPHome configuration for the XIAO Smart IR Mate, a smart device based on the Seeed Studio XIAO ESP32-C3. It enables IR remote control capabilities, RGB LED feedback, vibration output, and touch/button interactions, making it suitable for smart home automation and integration with Home Assistant.

## Features

- **WiFi Connectivity**: Connects to your WiFi network and supports fallback AP mode for easy setup.
- **IR Transmitter & Receiver**: Send and receive IR signals for controlling or learning from remote-controlled devices.
- **Touch Pad Input**: Detects single, double, and triple taps for different actions.
- **Reset Button**: Supports short and long press for device restart or factory reset.
- **RGB LED**: Visual feedback using a WS2812 RGB LED.
- **Vibration Output**: Haptic feedback via a vibration motor.
- **Home Assistant Integration**: Seamless integration using the ESPHome API.
- **OTA Updates**: Supports over-the-air firmware updates.

## Hardware Pinout

- **IR Transmitter**: GPIO3 (D1)
- **IR Receiver**: GPIO4 (D2)
- **Touch Pad**: GPIO5 (D3)
- **Vibration Output**: GPIO6 (D4)
- **RGB LED**: GPIO7 (D5)
- **Reset Button**: GPIO9 (D9)

## Usage

- **Single Tap**: Triggers the IR transmit action.
- **Double/Triple Tap**: Reserved for custom actions (can be extended in the YAML).
- **Short Press Reset Button**: Restarts the device.
- **Long Press Reset Button (5s+)**: Factory reset.

## Getting Started

1. **Install ESPHome**:  
   Follow the [ESPHome installation guide](https://esphome.io/guides/installing_esphome.html).

2. **Flash the Device**:  
   Use the following command to upload the configuration:
   ```bash
   esphome run xiao_smart_ir_mate.yaml
   ```

3. **Add to Home Assistant**:  
   The device will be discoverable via the ESPHome integration.

## Customization

You can modify the YAML file to:
- Change pin assignments.
- Add new actions for double/triple tap.
- Adjust LED or vibration behavior.


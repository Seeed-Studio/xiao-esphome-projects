# XIAO Soil Moisture Monitor

This project provides an ESPHome configuration for the XIAO Soil Moisture Monitor, a smart device based on the Seeed Studio XIAO ESP32-C6. It monitors soil moisture, provides three-color LED feedback, supports manual calibration, and features deep sleep for long battery life. The device is ideal for plant pots, planters, and other soil monitoring applications, with seamless Home Assistant integration.

## Features

- **Soil Moisture Monitoring**: Measures soil moisture using an analog sensor and reports status to Home Assistant.
- **Three-Color LED Feedback**: Indicates soil status (dry, almost dry, normal) with red, yellow, and green LEDs.
- **Manual Calibration**: Easily calibrate dry and wet values using the onboard button.
- **Deep Sleep**: Automatically enters deep sleep to save power, with wakeup on button press.
- **Battery Monitoring**: Monitors battery voltage and reports percentage.
- **WiFi Connectivity**: Connects to your WiFi network and supports fallback AP mode for easy setup.
- **Home Assistant Integration**: Seamless integration using the ESPHome API.
- **OTA Updates**: Supports over-the-air firmware updates.

## Hardware Pinout

- **Soil Sensor**: GPIO1
- **Battery Measurement**: GPIO0
- **Button**: GPIO2
- **LED Yellow**: GPIO18
- **LED Green**: GPIO19
- **LED Red**: GPIO20
- **PWM Output**: GPIO21

## Usage

- **Soil Status Indication**: The device blinks the appropriate LED (red/yellow/green) based on soil moisture level after each measurement.
- **Manual Calibration**:
  - Press the button once: Check soil moisture and update status.
  - Press the button three times quickly: Start calibration (first for dry, then for wet soil).
- **Deep Sleep**: Device sleeps between measurements to save power; wake up by pressing the button.
- **Battery Status**: Battery percentage is reported to Home Assistant.

## Getting Started

1. **Install ESPHome**:
   Follow the [ESPHome installation guide](https://esphome.io/guides/installing_esphome.html).

2. **Flash the Device**:
   Use the following command to upload the configuration:
   ```bash
   esphome run xiao-soil-moisture-monitor.yaml
   ```

3. **Add to Home Assistant**:
   The device will be discoverable via the ESPHome integration.

## Customization

You can modify the YAML file to:
- Change pin assignments or LED behavior.
- Adjust deep sleep and measurement intervals.
- Add automations or additional sensors.
- Integrate with other smart home systems.

# XIAO 2 Channel Wi-Fi AC Energy Meter

This project provides an ESPHome configuration for the XIAO 2 Channel Wi-Fi AC Energy Meter, a dual-channel electricity meter based on the Seeed Studio XIAO ESP32-C6 and BL0939. It enables high-precision measurement of current, voltage, power, and energy for two AC channels, with Home Assistant integration and web server support.

## Features

- **Dual-Channel Measurement**: Measures voltage, current, active power, and energy for two independent AC channels.
- **High Accuracy**: Uses BL0939 in current transformer mode for improved measurement precision.
- **Status LED**: Indicates device status via onboard LED.
- **Web Server**: Built-in web server for local monitoring.
- **WiFi Connectivity**: Connects to your WiFi network and supports fallback AP mode for easy setup.
- **Home Assistant Integration**: Seamless integration using the ESPHome API.
- **OTA Updates**: Supports over-the-air firmware updates.

## Hardware Pinout

- **Current Transformer UART TX**: GPIO16
- **Current Transformer UART RX**: GPIO17
- **Status LED**: GPIO19 (inverted)
- **External Antenna Enable**: GPIO14

## Usage

- **Real-Time Monitoring**: View voltage, current, power, and energy for both channels in Home Assistant or via the web server.
- **Status LED**: Always on by default to indicate device is powered and running.
- **Fallback AP**: If WiFi fails, the device starts a hotspot for configuration.

## Getting Started

1. **Install ESPHome**:
   Follow the [ESPHome installation guide](https://esphome.io/guides/installing_esphome.html).

2. **Flash the Device**:
   Use the following command to upload the configuration:
   ```bash
   esphome run xiao_2_channel_wifi_ac_energy_meter.yaml
   ```

3. **Add to Home Assistant**:
   The device will be discoverable via the ESPHome integration.

4. **Access Web Server**:
   After connecting to your network, access the device's IP address in your browser to view the web server dashboard.

## Customization

You can modify the YAML file to:
- Change pin assignments or LED behavior.
- Adjust sensor update intervals.
- Add automations or additional sensors.
- Integrate with other smart home systems.


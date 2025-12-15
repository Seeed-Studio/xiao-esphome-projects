# Seeed Studio XIAO W5500 Ethernet Adapter V1.2

The Seeed Studio XIAO W5500 Ethernet Adapter V1.2 is a compact expansion board designed for the Seeed Studio XIAO series. It empowers your XIAO projects with stable, hardwired Ethernet connectivity using the W5500 TCP/IP embedded Ethernet controller.

⚠️ Hardware Note: This configuration is strictly for V1.2 boards produced after November 1, 2025. Please check your hardware version before flashing.

## Features

- **Bluetooth Proxy**: Acts as a remote Bluetooth adapter for Home Assistant, extending the range of your BLE sensors.
- **PoE Ready**: Supports Power over Ethernet (PoE) for stable power supply.
- **Status LED**: Indicates device status via onboard LED.
- **Versatile I/O Options**: With dual-sided I/O outputs, you can easily expand functionality and interface with a range of sensors and peripherals to customize your application to exact specifications.
- **High-Performance Microcontroller**: Built on the XIAO ESP32S3 Plus platform, our board delivers robust processing power and efficient wireless connectivity, making it ideal for complex IoT and embedded applications.

## Hardware Pinout

- **CS**: GPIO2 (D1)
- **CLK**: GPIO7 (D8)
- **MISO**: GPIO8 (D9)
- **MOSI**: GPIO9 (D10)
- **INT**: GPIO10 (D2)

## Usage

- **Bluetooth Proxy**: Nearby Bluetooth devices are detectable within Home Assistant.
- **Short Press Reset Button**: Restarts the device.
- **Status LED**: Always on by default to indicate device is powered and running.
- **Power supply**: Use PoE or power the XIAO via  USB-C 5V  if your Ethernet is non-PoE.

## Getting Started

1. **Install ESPHome**:  
   Follow the [ESPHome installation guide](https://esphome.io/guides/installing_esphome.html).

2. **Flash the Device**:  
   Use the following command to upload the configuration:
   ```bash
   esphome run seeed-esp32-poe.yaml
   ```

3. **Add to Home Assistant**:  
   The device will be discoverable via the ESPHome integration.

4. **View Bluetooth Proxy**:
   Navigate to: Settings -> Devices & services -> Bluetooth -> (Gear icon)

## Customization

You can modify the YAML file to:
- Change pin assignments.
- Modify max_connections.
- Modify connection_slots.


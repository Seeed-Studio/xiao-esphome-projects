# Seeed Studio 6-Channel Relay based on XIAO ESP32-C6

## Overview

The **6-Channel Wi-Fi Relay Module** is a smart device from Seeed Studio designed to control multiple AC-powered appliances wirelessly. Its six-channel configuration supports independent control of six loads, making it an excellent choice for automating multiple household appliances such as lights, fans, and other devices in a Home Assistant environment.

This guide provides a detailed walkthrough, including setup, integration, and advanced configuration for users ranging from beginners to smart-home enthusiasts.

### Key Features and Specifications

<div class="table-center">
<table align="center">
    <thead>
        <tr>
            <th>Feature</th>
            <th>Details</th>
        </tr>
    </thead>
    <tbody>
        <tr>
            <th>Microcontroller</th>
            <td>ESP32-C6</td>
        </tr>
        <tr>
            <th>Channels</th>
            <td>6 (independent control for each channel)</td>
        </tr>
        <tr>
            <th>Connection Type</th>
            <td>Wi-Fi</td>
        </tr>
        <tr>
            <th>Flash Size</th>
            <td>4MB</td>
        </tr>
        <tr>
            <th>GPIO Pins</th>
            <td>Relay 1: GPIO2<br>Relay 2: GPIO21<br>Relay 3: GPIO1<br>Relay 4: GPIO0<br>Relay 5: GPIO19<br>Relay 6: GPIO18</td>
        </tr>
        <tr>
            <th>I2C Bus</th>
            <td>SDA: GPIO22, SCL: GPIO23</td>
        </tr>
        <tr>
            <th>Antenna</th>
            <td>External antenna support (RF switch control on GPIO3, GPIO14)</td>
        </tr>
    </tbody>
</table>
</div>

:::warning Safety Warning

Always disconnect AC power before wiring the relay. Avoid using the USB port while the device is connected to AC power to prevent electrical hazards.

:::

### Physical Layout and Connections

#### GPIO Pin Configuration

- **Relay Control Pins**:
  - Relay 1: GPIO2
  - Relay 2: GPIO21
  - Relay 3: GPIO1
  - Relay 4: GPIO0
  - Relay 5: GPIO19
  - Relay 6: GPIO18

- **I2C Bus**:
  - SDA: GPIO22
  - SCL: GPIO23

- **Antenna Control**:
  - RF Switch Control: GPIO3
  - External Antenna Select: GPIO14

#### Physical Representation

``` diagram
+-----------------------------------------+
| Seeed Studio 6-Channel Wi-Fi Relay      |
|-----------------------------------------|
| Relay 1: GPIO2                          |
| Relay 2: GPIO21                         |
| Relay 3: GPIO1                          |
| Relay 4: GPIO0                          |
| Relay 5: GPIO19                         |
| Relay 6: GPIO18                         |
| I2C: SDA(GPIO22), SCL(GPIO23)          |
+-----------------------------------------+
```

## Getting Started

### Requirements

1. **Core Components**:
   - 6-Channel Wi-Fi Relay Module (XIAO ESP32-C6 based)
2. **Software**:
   - Home Assistant installed
   - **ESPHome Add-On** set up for device communication (if not then the device will not be discovered)
3. **Network**:
   - Stable Wi-Fi for seamless interaction between hardware and Home Assistant.

### Step 1: Set Up the Relay Module (Physical Setup)

1. Wiring:
    - Connect your loads to the appropriate relay channels.
    - Ensure all connections are secure, and there are no exposed wires.
2. Power On:
    - Connect the device via USB or appropriate power supply.
    - Do not power on the module through USB if it is connected to AC power.

### Step 2: Network Configuration

1. **Enable Access Point**:
   - Upon powering up for the first time, the module will create a Wi-Fi network (SSID: `seeedstudio-6-channel-relay`).
2. **Access Configuration**:
   - Connect to the network using a phone or PC.
   - Open a browser and navigate to `http://192.168.4.1`.
   - Enter the SSID and password of your home Wi-Fi network.
3. **Home Assistant Integration**:
   - Once connected to the home network, the module will be discoverable in Home Assistant under `Settings -> Devices & Services`.

This way, you can connect the module to your Home Assistant network and let Home Assistant discover it.

### Step 3: Add the module device

1. **Automatic Discovery**:
   - Ensure the **ESPHome** is installed in Home Assistant.
   - Navigate to `Settings -> Devices & Services -> Integrations` and look for the device.
2. **Manual Configuration**:
   - If not automatically discovered, manually add the device by specifying its IP address.

## Configuration Details

### ESPHome Configuration

The device uses ESPHome with the following key features:

- **Platform**: ESP32-C6 (ESP-IDF framework)
- **Flash Size**: 4MB
- **Logging**: DEBUG level via USB Serial JTAG
- **OTA Updates**: Enabled for over-the-air firmware updates
- **Web Server**: Available on port 80
- **Captive Portal**: Enabled for easy Wi-Fi configuration

### Relay Control

Each relay can be controlled independently through Home Assistant switches:

- Relay 1
- Relay 2
- Relay 3
- Relay 4
- Relay 5
- Relay 6

## Automation and Use Cases

1. **Multi-Zone Lighting Control**:
   - Control lights in different rooms or zones independently.
2. **HVAC Management**:
   - Control multiple fans or heating elements in different areas.
3. **Time-Based Schedules**:
   - Create schedules for each relay to turn on/off at specific times.
4. **Sensor Integration**:
   - Combine the relay with temperature, motion, or other sensors for conditional control.
5. **Scene Management**:
   - Create scenes that control multiple relays simultaneously.
6. **Notifications**:
   - Set up Home Assistant to send notifications when relay states change or if the device goes offline.

## Advanced Features

### External Antenna Support

The device is configured to use an external antenna by default:
- RF switch control is initialized on boot
- GPIO3 and GPIO14 are configured for antenna selection
- This ensures optimal Wi-Fi signal strength

### I2C Bus

An I2C bus is available on GPIO22 (SDA) and GPIO23 (SCL) for connecting additional sensors or peripherals.

### OTA Updates

Over-the-air updates are enabled, allowing you to update the firmware without physical access to the device.

## Safety and Maintenance

1. Periodically inspect wiring for wear and tear.
2. Use proper circuit protection devices, such as fuses or breakers.
3. Keep the device away from water and excessive heat.
4. Ensure proper ventilation around the device.
5. Do not exceed the maximum load rating for each relay channel.

## Troubleshooting

### Device Not Connecting to Wi-Fi

1. Check if the access point is visible (SSID: `seeedstudio-6-channel-relay`).
2. Access the captive portal at `http://192.168.4.1`.
3. Verify Wi-Fi credentials are correct.

### Device Not Discovered in Home Assistant

1. Ensure ESPHome add-on is installed and running.
2. Check that the device and Home Assistant are on the same network.
3. Try manually adding the device using its IP address.

### Relay Not Responding

1. Check GPIO pin connections.
2. Verify the relay is receiving power.
3. Check Home Assistant logs for error messages.

## FAQs

1. **What happens if the device is disconnected from Wi-Fi?**
   - The module will attempt to reconnect automatically. If it fails, it will enable its fallback access point for reconfiguration.

2. **Can I use the USB port while connected to AC power?**
   - No, using USB while connected to AC power may damage the device or create a safety hazard.

3. **Can this module work without Home Assistant?**
   - Yes, the module can be controlled via its **local IP address** or integrated with other platforms that support ESPHome or MQTT.

4. **How many relays can I control simultaneously?**
   - All 6 relays can be controlled independently and simultaneously.

5. **Can I add more sensors to this device?**
   - Yes, the I2C bus (GPIO22/GPIO23) is available for connecting additional sensors or peripherals.

6. **What is the maximum load per relay?**
   - Please refer to the hardware specifications of your specific relay module for maximum load ratings.

## Version Information

- **Project Version**: 0.1
- **ESPHome Minimum Version**: 2024.3.2 (required to fix logger compile error on ESP32-C6)
- **Board**: ESP32-C6 DevKitC-1


# Seeed Studio 2 Channel Relay base on XIAO ESP32-C3

## Overview

The **[2-Channel Wi-Fi AC Relay Module](https://www.seeedstudio.com/Dual-Smart-Relay-Module-for-XIAO-p-6309.html)** is a smart device from Seeed Studio designed to control AC-powered appliances wirelessly. Its dual-channel configuration supports independent control of two loads, making it an excellent choice for automating household appliances such as lights, fans, and other devices in a Home Assistant environment.

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
            <th>Input Voltage</th>
            <td>AC 100-240V, 50/60Hz</td>
        </tr>
        <tr>
            <th>Output Voltage</th>
            <td>AC 100-240V, 50/60Hz</td>
        </tr>
        <tr>
            <th>Maximum Load</th>
            <td>10A per channel</td>
        </tr>
        <tr>
            <th>Channels</th>
            <td>2 (independent control for each channel)</td>
        </tr>
        <tr>
            <th>Connection Type</th>
            <td>Wi-Fi</td>
        </tr>
        <tr>
            <th>Input Terminals</th>
            <td>N (Neutral), L (Live)</td>
        </tr>
        <tr>
            <th rolspan="2">Output Terminals</th>
            <td><b>Channel 1:</b> N1 (Neutral), L1 (Live) <br></br> <b>Channel 2:</b> N2 (Neutral), L2 (Live)</td>
        </tr>
    </tbody>
</table>
</div>

:::warning Safety Warning

Always disconnect AC power before wiring the relay. Avoid using the USB port while the device is connected to AC power to prevent electrical hazards.

:::

### Physical Layout and Connections

#### Input and Output Terminals

- Input
  - **N (Neutral)**: Connect the neutral wire from the AC power supply.
  - **L (Live)**: Connect the live wire from the AC power supply.
- Output
  - Channel 1
    - **N1 (Neutral)**: Neutral wire for the load connected to Relay 1.
    - **L1 (Live)**: Live wire for the load connected to Relay 1.
  - Channel 2
    - **N2 (Neutral)**: Neutral wire for the load connected to Relay 2.
    - **L2 (Live)**: Live wire for the load connected to Relay 2.

#### Physical Representation

``` diagram
+-----------------------------------------+
| Seeed Studio 2-Channel AC Wi-Fi Relay   |
|-----------------------------------------|
| Input:                                  |
|   N  (Neutral)                          |
|   L  (Live)                             |
| Output: Channel 1                       |
|   N1 (Neutral for Load 1)               |
|   L1 (Live for Load 1)                  |
| Output: Channel 2                       |
|   N2 (Neutral for Load 2)               |
|   L2 (Live for Load 2)                  |
+-----------------------------------------+
```

## Getting Started

### Requirements

1. **Core Components**:
   - 2-Channel Wi-Fi AC Relay Module
2. **Software**:
   - Home Assistant installed
   - **ESPHome  Add-On** set up for device communication(if not then the device will not be discovered)
3. **Network**:
   - Stable Wi-Fi for seamless interaction between hardware and Home Assistant.

### Step 1: Set Up the Relay Module (Physical Setup)

1. Wiring:
    - Follow the Physical Layout and Connections section to wire the relay module.
    - Ensure all connections are secure, and there are no exposed wires.
2. Power On:
    - Turn on the AC supply. Do not power on the module through USB if it is connected to AC power.

### Step 2: Network Configuration

1. **Enable Access Point**:
   - Upon powering up for the first time, the module will create a Wi-Fi network (SSID: `seeedstudio-relay`).
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

## Automation and Use Cases

1. Basic Automation:
   - Create a time-based schedule to toggle the relay at specific times.
2. Sensor Integration:
   - Combine the relay with a temperature or motion sensor for conditional control (e.g., turn on a fan when the temperature exceeds a threshold).
3. Notifications:
   - Set up Home Assistant to send notifications when the relay state changes or if the device goes offline.

## Safety and Maintenance

1. Periodically inspect wiring for wear and tear.
2. Use proper circuit protection devices, such as fuses or breakers.
3. Keep the device away from water and excessive heat.

## FAQs

1. **What happens if the device is disconnected from Wi-Fi?**
   - The module will attempt to reconnect automatically. If it fails, it will enable its fallback access point for reconfiguration.
2. **Can I use the USB port while connected to AC power?**
   - No, using USB while connected to AC power may damage the device or create a safety hazard.
3. **Can this module work without Home Assistant?**
   - Yes, the module can be controlled via its **local IP address** or integrated with other platforms that support ESPHome or MQTT.

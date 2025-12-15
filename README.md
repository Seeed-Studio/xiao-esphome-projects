# XIAO ESPHome Projects

> ## 🚀 Quick Firmware Installation
>
> ### ✨ Visit our Web Installer: [https://gadgets.seeed.cc](https://gadgets.seeed.cc) ✨
>
> Simply click the link to easily install firmware directly to your device!

Welcome to the XIAO ESPHome Projects repository! This repository contains a collection of projects and examples using the Seeed Studio XIAO series with ESPHome. These projects demonstrate various capabilities and integrations of XIAO boards with ESPHome, making it easier for you to get started with IoT development.

## Table of Contents

- [XIAO ESPHome Projects](#xiao-esphome-projects)
  - [Table of Contents](#table-of-contents)
  - [Introduction](#introduction)
  - [Getting Started](#getting-started)
  - [Projects](#projects)
    - [Project 1: XIAO 2 Channel Wi-Fi AC Energy Meter](#project-1-xiao-2-channel-wi-fi-ac-energy-meter)
    - [Project 2: XIAO Soil Moisture Monitor](#project-2-xiao-soil-moisture-monitor)
    - [Project 3: Seeed Studio IoT Button](#project-3-seeed-studio-iot-button)
    - [Project 4: XIAO Smart IR Mate](#project-4-xiao-smart-ir-mate)
    - [Project 5: XIAO 24GHz mmWave](#project-5-xiao-24ghz-mmwave)
    - [Project 6: Seeed Studio XIAO W5500 Ethernet Adapter V1.2](#project-6-seeed-esp32-poe)
  - [Installation](#installation)
    - [📱 Web Installer - One-Click Firmware Installation](#-web-installer---one-click-firmware-installation)
  - [Contributing](#contributing)
  - [License](#license)

## Introduction

This repository provides a wide range of projects that leverage the power of Xiao boards with ESPHome. Whether you are a beginner or an experienced developer, you will find useful examples and detailed instructions to help you build your own IoT applications.

## Getting Started

To get started with the projects in this repository, you will need:

- Seeed Studio Xiao board
- ESPHome installed on your system
- Basic knowledge of YAML and ESPHome configurations

Follow the instructions in each project folder to set up and deploy the projects.

## Projects

### Project 1: XIAO 2 Channel Wi-Fi AC Energy Meter

XIAO 2 Channel Wi-Fi AC Energy Meter is a dual-channel electricity meter based on bl0939, which can measure electrical parameters such as current, voltage, and power. SeeedStudio-2CH-EM adopts the current transformer solution of bl0939, which can provide higher electrical measurement accuracy.

### Project 2: XIAO Soil Moisture Monitor

XIAO Soil Moisture Monitor is used to monitor soil moisture and feedback the current status through a three-color LED. It supports manual calibration and automatic deep sleep to save energy. It is suitable for scenes such as flower pots and planting boxes.

### Project 3: Seeed Studio IoT Button

The Seeed Studio IoT Button is a multifunctional Internet of Things (IoT) device that can be seamlessly integrated with smart home systems. It supports both ESPHome and Zigbee connectivity options, enhancing its versatility and usability.
**Supported Versions & Recommendations:**

- **IoT Button V1 (First Generation):**
  - ESPHome (WiFi, Home Assistant integration)
  - ESPHome (MQTT, any MQTT broker) **&larr; Recommended for V1**
  - Arduino with Zigbee

- **IoT Button V2 (Second Generation):**
  - ESPHome (WiFi, Home Assistant integration, battery monitoring)
  - Arduino with Zigbee (battery monitoring, deep sleep) **&larr; Recommended for V2**

For details and usage, see [projects/seeedstudio-iot-button/README.md](projects/seeedstudio-iot-button/README.md).

### Project 4: XIAO Smart IR Mate

XIAO Smart IR Mate is a smart device based on the Seeed Studio XIAO ESP32-C3. It enables IR remote control capabilities, RGB LED feedback, vibration output, and touch/button interactions, making it suitable for smart home automation and integration with Home Assistant.

### Project 5: XIAO 24GHz mmWave

XIAO 24GHz mmWave is an ESPHome project for the Seeed Studio XIAO ESP32-C6 board equipped with a 24GHz mmWave radar sensor (LD2410). It features deep sleep, battery monitoring, and Home Assistant integration, making it ideal for low-power, battery-operated presence detection.

### Project 6: Seeed Studio XIAO W5500 Ethernet Adapter V1.2

Seeed Studio XIAO W5500 Ethernet Adapter V1.2 enables the XIAO ESP32S3 to operate as a high-performance Bluetooth Proxy for Home Assistant.By utilizing a stable Ethernet connection for data backhaul instead of Wi-Fi, this setup eliminates wireless interference, freeing up the ESP32's radio strictly for scanning Bluetooth Low Energy (BLE) devices. 

## Installation

### 📱 Web Installer - One-Click Firmware Installation

<div align="center">
  <h3><a href="https://gadgets.seeed.cc">https://gadgets.seeed.cc</a></h3>
  <p>👆 Click the link above to access our Web Installer 👆</p>
  <p>No complex setup required - install firmware directly through your browser!</p>
</div>

## Contributing

We welcome contributions from the community! If you have a project or improvement to share, please follow these steps:

1. Fork the repository
2. Create a new branch (`git checkout -b feature/your-feature`)
3. Commit your changes (`git commit -am 'Add new feature'`)
4. Push to the branch (`git push origin feature/your-feature`)
5. Create a new Pull Request

## License

Since [esphome](https://github.com/esphome/esphome) is licensed under the GPLv3 license, this project adheres to open-source regulations and continues to maintain the GPLv3 license. For more details, please feel free to check the [LICENSE](LICENSE) file.

---

Thank you for visiting our repository! If you find the projects helpful, please give us a star ⭐ and share it with others.

# Seeed Studio IoT Button

This repository contains firmware implementations for the Seeed Studio IoT Button, supporting both ESPHome and Zigbee connectivity options.

## Project Overview

The Seeed Studio IoT Button is a versatile IoT device that can be integrated with smart home systems. This project provides two implementation options:

1. **ESPHome Implementation**: Configure the button as an ESPHome device for direct integration with Home Assistant.
2. **Zigbee Implementation**: Use the button as a Zigbee device for integration with any Zigbee-compatible hub.

## Features

- Multiple button actions: single click, double click, and long press
- Easy integration with Home Assistant
- Web-based firmware installation using ESP Web Tools
- Automatic firmware builds via GitHub Actions

## ESPHome Implementation

The ESPHome configuration (`seeed_iot_button_example.yaml`) provides:

- Button press detection with multiple actions
- Virtual switches that can be toggled by different button actions
- Wi-Fi connectivity with fallback access point
- OTA update capability

## Zigbee Implementation

The Zigbee implementation (`Zigbee_Seeed_IoT_Button/`) includes:

- Arduino sketch for ESP32-C6 with Zigbee capability
- Button event detection (single click, double click, long press)
- Zigbee endpoint configuration for smart home integration
- Factory reset capability via long press

## Installation

You can install the firmware directly to your device using the web installer available at the project's GitHub Pages site.

## Development

To modify the firmware:

1. For ESPHome: Edit the `seeed_iot_button_example.yaml` file
2. For Zigbee: Modify the Arduino sketch in the `Zigbee_Seeed_IoT_Button` directory

## CI/CD

This project uses GitHub Actions to:

- Build the firmware automatically
- Deploy the web installer to GitHub Pages
- Run continuous integration tests

## License

MIT

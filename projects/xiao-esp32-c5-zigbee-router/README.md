# XIAO ESP32-C5 Zigbee Router (ESP-IDF)

An independently tested Zigbee router firmware for Seeed Studio XIAO ESP32-C5. Unlike the ESPHome YAML projects in this repository, this project uses Espressif's native ESP-IDF Zigbee stack. It joins an existing Zigbee network as a router; it is not a coordinator and does not expose Wi-Fi or ESPHome entities.

## Features

- Zigbee router / range extender role; Home Assistant ZHA tested.
- Standard Home Automation Range Extender endpoint with Basic and Identify clusters.
- ZCL Basic attributes report manufacturer `Seeed Studio` and model `XIAO ESP32-C5`.
- BOOT short press (GPIO28) requests an updated coordinator neighbor/LQI table. The Identify **Press** diagnostic action in Home Assistant also generates Zigbee traffic to refresh link readings.
- Hold BOOT for 8 seconds to invoke the Zigbee local reset/rejoin action. Enable permit-join on the coordinator before using it.

## Hardware

- Seeed Studio XIAO ESP32-C5
- USB cable for power and flashing
- An existing Zigbee coordinator (tested with Home Assistant ZHA)

No external peripheral wiring is required. Keep the device USB-powered or otherwise mains-powered for router operation.

## Build and flash

Tested with ESP-IDF 6.2.0 preview support, target `esp32c5`, and ESP Zigbee/ZBOSS libraries 1.6.9. Install ESP-IDF and its component manager, then run:

```sh
cd projects/xiao-esp32-c5-zigbee-router
idf.py --preview set-target esp32c5
idf.py --preview build
idf.py --preview -p PORT flash monitor
```

Replace `PORT` with the serial port for your XIAO. Enable permit-join on the coordinator before first boot. A factory-new device starts network steering automatically and retries if joining is not yet available. If the device was paired with a different coordinator, erase flash/NVS before commissioning it again.

> This firmware uses ESP-IDF directly; it is not currently an ESPHome YAML firmware and is not included in the repository's ESPHome build matrix or one-click web installer.

## Home Assistant ZHA verification

1. In **Settings → Devices & services → ZHA**, start **Add device** while permit-join is enabled.
2. Confirm the new device appears as a Router and remains available after reboot.
3. In the ZHA device page, read Basic cluster attributes `manufacturer` (0x0004) and `model` (0x0005). Expected values: `Seeed Studio` and `XIAO ESP32-C5`.
4. Press BOOT briefly or use the Identify diagnostic **Press** action, then check the updated link metrics in the coordinator UI. RSSI/LQI depend on placement, antenna, channel, and interference; a single reading is not a range guarantee.

## Tested configuration and results

- Board: Seeed Studio XIAO ESP32-C5.
- Host: Home Assistant OS 2026.11.1 on Raspberry Pi.
- Integration: ZHA 2.4.3.
- Firmware build: ESP-IDF 6.2.0 preview target `esp32c5`; ESP Zigbee/ZBOSS 1.6.9. Build completed and firmware was flashed over USB.
- Join/operation: discovered through ZHA and identified as a mains-powered Router; it remained available after reboot. Identify diagnostic control was exposed.
- Basic attributes: initially displayed `None` because the ZCL character strings lacked their required length prefix. After adding the prefixes, the user confirmed both reads worked.
- One observed link sample: LQI 255, RSSI -24 dBm. This is one environment-specific measurement.

This is a single-prototype functional test, not certification or a broad interoperability claim. Machine-specific `sdkconfig`, generated build output, flash images, and the board's MAC address are intentionally omitted. Project files are covered by the repository's GPL-3.0 license; see the repository-level `LICENSE`.

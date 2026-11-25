# XIAO ESP32-C5 Zigbee Router (ESP-IDF)

An independently tested Zigbee router firmware for Seeed Studio XIAO ESP32-C5. Unlike the ESPHome YAML projects in this repository, this project uses Espressif's native ESP-IDF Zigbee stack. It joins an existing Zigbee network as a router; it is not a coordinator and does not expose Wi-Fi or ESPHome entities.

## Features

- Zigbee router / range extender role; Home Assistant ZHA tested.
- Standard Home Automation Range Extender endpoint with Basic and Identify clusters.
- ZCL Basic attributes report manufacturer `Seeed Studio` and model `XIAO ESP32-C5`.
- BOOT short press (GPIO28) requests an updated coordinator neighbor/LQI table. The Identify **Press** diagnostic action in Home Assistant also generates Zigbee traffic to refresh link readings.
- Hold BOOT for 8 seconds to leave the current network and clear the Zigbee settings (local reset). The router then reboots and starts network steering again, so enable permit-join on the coordinator first.

## Hardware

- Seeed Studio XIAO ESP32-C5
- USB cable for power and flashing
- An existing Zigbee coordinator (tested with Home Assistant ZHA)

No external peripheral wiring is required. Keep the device USB-powered or otherwise mains-powered for router operation.

## Build and flash

Built with ESP-IDF v6.1.1 for `esp32c5`, with ESP Zigbee/ZBOSS libraries 1.6.9 pinned in `dependencies.lock`. The project also builds unchanged on ESP-IDF v5.5.1. Install ESP-IDF and its component manager, then run:

```sh
cd projects/xiao-esp32-c5-zigbee-router
idf.py set-target esp32c5
idf.py build
idf.py -p PORT flash monitor
```

Replace `PORT` with the serial port for your XIAO. Enable permit-join on the coordinator before first boot. A factory-new device starts network steering automatically and retries every 10 seconds if joining is not yet available. If the device was paired with a different coordinator, hold BOOT for 8 seconds (or erase flash/NVS) before commissioning it again.

> This firmware uses ESP-IDF directly; it is not currently an ESPHome YAML firmware and is not included in the repository's ESPHome build matrix or one-click web installer. The repository CI builds it in a separate job.

## Home Assistant ZHA verification

1. In **Settings → Devices & services → ZHA**, start **Add device** while permit-join is enabled.
2. Confirm the new device appears as a Router and remains available after reboot.
3. In the ZHA device page, read Basic cluster attributes `manufacturer` (0x0004) and `model` (0x0005). Expected values: `Seeed Studio` and `XIAO ESP32-C5`.
4. Press BOOT briefly or use the Identify diagnostic **Press** action, then check the updated link metrics in the coordinator UI. RSSI/LQI depend on placement, antenna, channel, and interference; a single reading is not a range guarantee.

## Tested configuration and results

The hardware results below come from an early prototype build (ESP-IDF 6.2 development branch), before the firmware was updated to reboot and re-steer after a leave and to build on ESP-IDF v6.1.1. The current sources are built by the repository CI, but the current binary has not been re-tested on a board yet.

- Board: Seeed Studio XIAO ESP32-C5.
- Host: Home Assistant OS 2026.11.1 on Raspberry Pi.
- Integration: ZHA 2.4.3.
- Join/operation: discovered through ZHA and identified as a mains-powered Router; it remained available after reboot. Identify diagnostic control was exposed.
- Basic attributes: ZHA reads back `Seeed Studio` and `XIAO ESP32-C5`.
- One observed link sample: LQI 255, RSSI -24 dBm. This is one environment-specific measurement.
- Not verified on hardware yet: the link refresh triggered by a BOOT short press or by the Identify action, and the 8-second local reset with re-steering.

This is a single-prototype functional test, not certification or a broad interoperability claim. Machine-specific `sdkconfig`, generated build output, flash images, and the board's MAC address are intentionally omitted. Project files are covered by the repository's GPL-3.0 license; see the repository-level `LICENSE`.

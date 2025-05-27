# XIAO 24GHz mmWave ESPHome Project

This project provides an ESPHome configuration for the Seeed Studio XIAO ESP32-C6 board, equipped with a 24GHz mmWave radar sensor (LD2410). The configuration enables deep sleep, battery monitoring, and Home Assistant integration, making it ideal for low-power, battery-operated presence detection.

## Features

- **ESP32-C6 Support:** Uses the latest ESP-IDF framework for the XIAO ESP32-C6 board.
- **LD2410 mmWave Radar:** Integrates the LD2410 sensor for presence and motion detection.
- **Battery Monitoring:** Reads battery voltage via ADC and reports to Home Assistant.
- **Deep Sleep:** Automatically manages deep sleep for power saving, with wakeup on GPIO2.
- **Home Assistant API:** Seamless integration with Home Assistant via native API.
- **OTA Updates:** Supports over-the-air firmware updates.
- **Fallback WiFi Hotspot:** Captive portal for easy setup if WiFi connection fails.
- **Custom Switches:** Controls power to mmWave, RF, and ADC circuits via GPIO.

## Hardware Required

- Seeed Studio XIAO ESP32-C6 board
- LD2410 24GHz mmWave radar sensor
- Battery (for portable operation)
- Optional: Additional circuitry for power switching

## Wiring

| XIAO Pin | Function         | Description         |
|----------|------------------|--------------------|
| GPIO16   | UART TX          | LD2410 RX          |
| GPIO17   | UART RX          | LD2410 TX          |
| GPIO1    | ADC Input        | Battery voltage    |
| GPIO19   | Power Output     | mmWave enable      |
| GPIO3    | RF Output        | RF enable          |
| GPIO20   | ADC Output       | ADC enable         |
| GPIO2    | Wakeup Pin       | Deep sleep wakeup  |

## Setup Instructions

1. **Clone the Project:**
   ```sh
   git clone https://github.com/Seeed-Studio/xiao-esphome-projects
   cd xiao-esphome-projects/projects/xiao_24ghz_mmwave
   ```

2. **Install ESPHome:**
   ```sh
   pip install esphome
   ```

3. **Configure WiFi:**
   Edit `xiao_24ghz_mmwave.yaml` and add your WiFi credentials under the `wifi:` section:
   ```yaml
   wifi:
     ssid: "YOUR_WIFI_SSID"
     password: "YOUR_WIFI_PASSWORD"
     ap:
       ssid: "${friendly_name}"
   ```

4. **Compile and Upload:**
   Connect your XIAO ESP32-C6 via USB and run:
   ```sh
   esphome run xiao_24ghz_mmwave.yaml
   ```

5. **Add to Home Assistant:**
   - Use the ESPHome integration in Home Assistant to add the device.
   - The device will expose sensors and switches for presence, battery, and power control.

## Customization

- **Deep Sleep Settings:** Adjust `run_duration` and `sleep_duration` under the `deep_sleep:` section to balance responsiveness and battery life.
- **GPIO Assignments:** Change pin numbers in the YAML if your hardware wiring differs.
- **Sensor Filters:** Modify the ADC filter lambda for different battery voltage dividers.

## External Components

This configuration uses external components from:
- [ESPHome PR #7942](https://github.com/esphome/esphome/pull/7942) for ADC improvements.
- [Seeed Studio xiao-esphome-projects](https://github.com/Seeed-Studio/xiao-esphome-projects) for custom API and LD2410 support.

## Troubleshooting

- If WiFi fails, the device will start a fallback hotspot for configuration.
- Use the OTA update feature for wireless firmware updates after the first flash.


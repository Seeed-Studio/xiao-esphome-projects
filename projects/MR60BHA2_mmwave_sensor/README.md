# Seeed Studio MR60BHA2 mmWave Sensor ESPHome Project

This project provides an ESPHome configuration for the Seeed Studio MR60BHA2 Kit, a 60GHz mmWave radar sensor module based on the XIAO ESP32-C6 board. The MR60BHA2 is designed for non-contact vital signs monitoring, capable of detecting human presence, measuring real-time heart rate and respiratory rate, and calculating distance to detection objects.

## Features

- **ESP32-C6 Support:** Uses ESP-IDF framework for the XIAO ESP32-C6 board with 4MB flash.
- **Vital Signs Monitoring:** Real-time detection of heart rate and respiratory rate.
- **Presence Detection:** Binary sensor for person information detection.
- **Distance Measurement:** Measures distance to detection objects.
- **Target Counting:** Reports the number of detected targets.
- **Ambient Light Sensor:** BH1750 illuminance sensor for environmental monitoring.
- **RGB LED Indicator:** WS2812 RGB LED for visual feedback.
- **Home Assistant Integration:** Seamless integration with Home Assistant via native API.
- **OTA Updates:** Supports over-the-air firmware updates.
- **Fallback WiFi Hotspot:** Captive portal for easy setup if WiFi connection fails.

## Hardware Required

- Seeed Studio XIAO ESP32-C6 board
- Seeed Studio MR60BHA2 60GHz mmWave radar sensor module
- BH1750 ambient light sensor (optional, included in kit)
- WS2812 RGB LED (optional, included in kit)

## Wiring

| XIAO Pin | Function         | Description                    |
|----------|------------------|--------------------------------|
| GPIO16   | UART TX          | MR60BHA2 RX                    |
| GPIO17   | UART RX          | MR60BHA2 TX                    |
| GPIO1    | RGB LED          | WS2812 data pin                |
| GPIO22   | I2C SDA          | BH1750 SDA                     |
| GPIO23   | I2C SCL          | BH1750 SCL                     |

## Sensors and Entities

### Binary Sensors
- **Person Information:** Detects if a person/target is present in the detection range.

### Sensors
- **Real-time Respiratory Rate:** Measures breathing rate in breaths per minute.
- **Real-time Heart Rate:** Measures heart rate in beats per minute.
- **Distance to Detection Object:** Distance measurement in centimeters.
- **Target Number:** Number of targets detected by the radar.
- **Illuminance:** Ambient light level from BH1750 sensor (lux).

### Lights
- **RGB Light:** WS2812 RGB LED for status indication and visual feedback.

## Setup Instructions

1. **Clone the Project:**
   ```sh
   git clone https://github.com/Seeed-Studio/xiao-esphome-projects
   cd xiao-esphome-projects/projects/MR60BHA2_mmwave_sensor
   ```

2. **Install ESPHome:**
   ```sh
   pip install esphome
   ```
   Or use Home Assistant ESPHome add-on.

3. **Configure WiFi:**
   Edit `MR60BHA2_mmwave_sensor.yaml` and add your WiFi credentials:
   ```yaml
   wifi:
     ssid: "YOUR_WIFI_SSID"
     password: "YOUR_WIFI_PASSWORD"
   ```

4. **Compile and Upload:**
   Connect your XIAO ESP32-C6 via USB and run:
   ```sh
   esphome run MR60BHA2_mmwave_sensor.yaml
   ```

5. **Add to Home Assistant:**
   - The device will be automatically discoverable via the ESPHome integration.
   - All sensors and binary sensors will be available in Home Assistant.

## External Components

This configuration uses an external component:
- **seeed_mr60bha2:** Custom ESPHome component for MR60BHA2 sensor
  - Source: [MR60BHA2_ESPHome_external_components](https://github.com/limengdu/MR60BHA2_ESPHome_external_components)

## Usage

- **Vital Signs Monitoring:** Place the sensor near a person (recommended distance: 0.5-2 meters) to monitor heart rate and respiratory rate.
- **Presence Detection:** Use the binary sensor to detect if someone is in the detection area.
- **Distance Measurement:** Monitor the distance sensor to track how far objects are from the sensor.
- **Target Counting:** The sensor can detect and count multiple targets in its field of view.

## Customization

- **UART Settings:** Adjust baud rate, pins, or parity if needed (default: 115200, GPIO16/17).
- **I2C Configuration:** Modify I2C pins if your wiring differs (default: GPIO22/23).
- **LED Configuration:** Change RGB LED pin or number of LEDs (default: GPIO1, 1 LED).
- **Update Intervals:** Adjust sensor update intervals in the YAML file.
- **Logging Level:** Change logger level from DEBUG to INFO or WARN for production use.

## Troubleshooting

- **No Vital Signs Data:** Ensure the person is within the recommended detection range (0.5-2 meters) and relatively still.
- **WiFi Connection Issues:** The device will start a fallback hotspot named "seeedstudio-mr60bha2" for configuration.
- **Sensor Not Responding:** Check UART wiring and ensure the MR60BHA2 module is properly powered.
- **I2C Errors:** Verify BH1750 sensor wiring and I2C address (default: 0x23).

## Technical Specifications

- **Board:** ESP32-C6 DevKitC-1
- **Flash Size:** 4MB
- **Framework:** ESP-IDF
- **UART Baud Rate:** 115200
- **I2C Address (BH1750):** 0x23
- **Minimum ESPHome Version:** 2024.3.2

## License

See the main repository LICENSE file for details.


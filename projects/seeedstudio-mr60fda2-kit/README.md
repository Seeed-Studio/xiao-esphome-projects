# Seeed Studio MR60FDA2 mmWave Sensor ESPHome Project

This project provides an ESPHome configuration for the Seeed Studio MR60FDA2 Kit, a 60GHz mmWave radar sensor module based on the XIAO ESP32-C6 board. The MR60FDA2 is designed for fall detection and presence monitoring, making it ideal for elderly care, smart home automation, and security applications.

## Features

- **ESP32-C6 Support:** Uses ESP-IDF framework for the XIAO ESP32-C6 board with 4MB flash.
- **Fall Detection:** Advanced algorithm to detect falls and alert caregivers.
- **Presence Detection:** Binary sensor for detecting people in the monitoring area.
- **Configurable Parameters:** Adjustable install height, height threshold, and sensitivity settings.
- **Ambient Light Sensor:** BH1750 illuminance sensor for environmental monitoring.
- **RGB LED Indicator:** WS2812 RGB LED for visual feedback and status indication.
- **Power Management:** Optimized power settings for low-power operation.
- **Home Assistant Integration:** Seamless integration with Home Assistant via native API.
- **OTA Updates:** Supports over-the-air firmware updates.
- **Fallback WiFi Hotspot:** Captive portal for easy setup if WiFi connection fails.

## Hardware Required

- Seeed Studio XIAO ESP32-C6 board
- Seeed Studio MR60FDA2 60GHz mmWave radar sensor module
- BH1750 ambient light sensor (optional, included in kit)
- WS2812 RGB LED (optional, included in kit)

## Wiring

| XIAO Pin | Function         | Description                    |
|----------|------------------|--------------------------------|
| GPIO16   | UART TX          | MR60FDA2 RX                    |
| GPIO17   | UART RX          | MR60FDA2 TX                    |
| GPIO1    | RGB LED          | WS2812 data pin                |
| GPIO22   | I2C SDA          | BH1750 SDA                     |
| GPIO23   | I2C SCL          | BH1750 SCL                     |

## Sensors and Entities

### Binary Sensors
- **Person Information:** Detects if a person is present in the detection range.
- **Falling Information:** Triggers when a fall is detected by the radar sensor.

### Sensors
- **Illuminance:** Ambient light level from BH1750 sensor (lux).

### Buttons
- **Get Radar Parameters:** Retrieves current radar configuration parameters.
- **Reset:** Factory reset button to restore default settings.

### Selects
- **Set Install Height:** Configure the installation height of the sensor (affects fall detection accuracy).
- **Set Height Threshold:** Adjust the height threshold for fall detection.
- **Set Sensitivity:** Configure detection sensitivity levels.

### Lights
- **RGB Light:** WS2812 RGB LED for status indication and visual feedback.

## Setup Instructions

1. **Clone the Project:**
   ```sh
   git clone https://github.com/Seeed-Studio/xiao-esphome-projects
   cd xiao-esphome-projects/projects/seeedstudio-mr60fda2-kit
   ```

2. **Install ESPHome:**
   ```sh
   pip install esphome
   ```
   Or use Home Assistant ESPHome add-on.

3. **Configure WiFi:**
    Edit `seeedstudio-mr60fda2-kit.yaml` and add your WiFi credentials:
   ```yaml
   wifi:
     ssid: "YOUR_WIFI_SSID"
     password: "YOUR_WIFI_PASSWORD"
   ```

4. **Compile and Upload:**
   Connect your XIAO ESP32-C6 via USB and run:
   ```sh
   esphome run seeedstudio-mr60fda2-kit.yaml
   ```

5. **Add to Home Assistant:**
   - The device will be automatically discoverable via the ESPHome integration.
   - All sensors, binary sensors, buttons, and selects will be available in Home Assistant.

6. **Configure Sensor Parameters:**
   - After adding to Home Assistant, use the select entities to configure:
     - **Install Height:** Set according to your mounting height (typically 2-3 meters for ceiling mount).
     - **Height Threshold:** Adjust based on your use case and room layout.
     - **Sensitivity:** Fine-tune detection sensitivity to reduce false positives.

## External Components

This configuration uses external components:
- **adc:** Custom ADC component from ESPHome fork
  - Source: [ssieb/esphome](https://github.com/ssieb/esphome) (ref: adc)
- **seeed_mr60fda2:** Custom ESPHome component for MR60FDA2 sensor
  - Source: [MR60FDA2_ESPHome_external_components](https://github.com/limengdu/MR60FDA2_ESPHome_external_components)

## Usage

### Fall Detection Setup
1. **Mounting:** Install the sensor on the ceiling or wall at a height of 2-3 meters, pointing downward.
2. **Calibration:** Use the "Get Radar Parameters" button to check current settings.
3. **Configuration:**
   - Set **Install Height** to match your actual mounting height.
   - Adjust **Height Threshold** based on typical person height in your area.
   - Fine-tune **Sensitivity** to balance detection accuracy and false positive rate.
4. **Testing:** Walk around the detection area and verify presence detection works correctly.
5. **Fall Detection:** The sensor will automatically detect falls and trigger the "Falling Information" binary sensor.

### Presence Detection
- The "Person Information" binary sensor will be ON when someone is detected in the monitoring area.
- Use this for occupancy-based automations in Home Assistant.

### Automation Examples
- **Fall Alert:** Create an automation that sends a notification when fall is detected.
- **Occupancy Lighting:** Turn on lights when person is detected.
- **Security Monitoring:** Alert when unexpected presence is detected.

## Power Management

This configuration includes optimized power management settings for the ESP32-C6:
- **Unicore Mode:** Enabled for lower power consumption.
- **Tickless Idle:** Reduces power usage during idle periods.
- **Light Sleep:** Automatic light sleep when possible.
- **CPU Frequency:** Set to 80MHz for balanced performance and power consumption.

## Customization

- **UART Settings:** Adjust baud rate, pins, or parity if needed (default: 115200, GPIO16/17).
- **I2C Configuration:** Modify I2C pins if your wiring differs (default: GPIO22/23).
- **LED Configuration:** Change RGB LED pin or number of LEDs (default: GPIO1, 1 LED).
- **Update Intervals:** Adjust sensor update intervals in the YAML file.
- **Logging Level:** Change logger level from DEBUG to INFO or WARN for production use.
- **Power Settings:** Modify `sdkconfig_options` to adjust power management behavior.

## Troubleshooting

- **False Fall Detections:** 
  - Adjust sensitivity to a lower value.
  - Verify install height is correctly set.
  - Check that the sensor is mounted securely and not vibrating.
- **No Presence Detection:**
  - Verify the sensor is pointing at the correct area.
  - Check that install height matches actual mounting height.
  - Increase sensitivity if needed.
- **WiFi Connection Issues:** The device will start a fallback hotspot named "seeedstudio-mr60fda2" for configuration.
- **Sensor Not Responding:** Check UART wiring and ensure the MR60FDA2 module is properly powered.
- **I2C Errors:** Verify BH1750 sensor wiring and I2C address (default: 0x23).

## Technical Specifications

- **Board:** ESP32-C6 DevKitC-1
- **Flash Size:** 4MB
- **Framework:** ESP-IDF
- **UART Baud Rate:** 115200
- **I2C Address (BH1750):** 0x23
- **Minimum ESPHome Version:** 2024.3.2
- **Power Management:** Optimized for low-power operation with light sleep support

## Application Scenarios

- **Elderly Care:** Monitor elderly family members and receive alerts for falls.
- **Smart Home:** Occupancy-based lighting and climate control.
- **Security:** Presence detection for home security systems.
- **Healthcare:** Non-intrusive monitoring in healthcare facilities.

## License

See the main repository LICENSE file for details.


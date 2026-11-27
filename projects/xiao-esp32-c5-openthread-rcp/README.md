# XIAO ESP32-C5 USB OpenThread RCP

Native ESP-IDF firmware that turns a Seeed Studio XIAO ESP32-C5 into an
OpenThread Radio Co-Processor (RCP). A host such as a Raspberry Pi running
Home Assistant OS provides OpenThread Border Router (OTBR); this board supplies
the IEEE 802.15.4 radio over its native USB Serial/JTAG port.

```text
Home Assistant OS / OTBR host -- USB -- XIAO ESP32-C5 RCP -- Thread devices
```

This is a radio-only project, not an OTBR running on the XIAO, a Matter end
device, or a Zigbee coordinator/router. Wi-Fi and ESPHome are not used.
Thread uses the 2.4 GHz radio; the board's 5 GHz Wi-Fi is not involved.

## Hardware and firmware defaults

- Seeed Studio XIAO ESP32-C5 with its supplied external antenna attached.
- A USB data cable; connect directly to the OTBR host for normal operation.
- Native USB RCP transport selected by default: no UART jumper wires,
  external USB-to-UART adapter, or manual menuconfig change is required.
- 8 MB flash, as specified in the [Seeed board documentation](https://wiki.seeedstudio.com/xiao_esp32c5_getting_started/).
  The factory app partition is 1 MB; the remaining flash is not needed by RCP.
- Application, bootloader and OpenThread logging are disabled to keep the
  USB Spinel channel free of text output. No physical-button or LED application
  behavior is added; BOOT/RESET retain their normal flashing functions.

## Build and flash

Use released **ESP-IDF v5.5.2**, including its submodules. The OpenThread stack
comes from that IDF release; there are no managed external components or
machine-specific component paths to lock. This is a standalone ESP-IDF project,
not part of the ESPHome matrix or the one-click web installer.

If ESP-IDF is not installed:

```sh
git clone --branch v5.5.2 --recursive https://github.com/espressif/esp-idf.git
cd esp-idf
./install.sh esp32c5
. ./export.sh
```

In the same shell, change to this repository, then run:

```sh
cd projects/xiao-esp32-c5-openthread-rcp
idf.py set-target esp32c5
idf.py build
python tools/check_build.py build
idf.py -p PORT flash
```

Replace `PORT` with the actual device path, such as `/dev/cu.usbmodem...` on
macOS or `/dev/ttyACM0` on Linux. To enter download mode, hold BOOT, tap and
release RESET, then release BOOT. After flashing, tap RESET once to start the
application. Flashing replaces the previous application; save any firmware or
settings you need before changing the board's role.

`tools/check_build.py` checks the **resolved** build configuration and flash
arguments: ESP32-C5, 8 MB flash, USB Radio Only mode and silent logging. This
catches a default being silently ignored by Kconfig. The dedicated CI workflow
builds in `espressif/idf:v5.5.2` and runs the same checks.

Do not use `idf.py monitor` while OTBR owns the port. This interface carries
binary Spinel frames, not a human-readable CLI. The firmware does not create
its own Thread dataset: network credentials and commissioning belong to the host.

## Home Assistant OS setup

1. Connect the flashed XIAO to the Raspberry Pi or other HA OS host by USB.
2. Install the official **OpenThread Border Router** app from Settings > Apps
   (called Add-ons in older HA versions), rather than manually adding the
   similarly named integration and entering a REST URL.
3. In the app configuration, select the XIAO's serial device, preferably its
   stable `/dev/serial/by-id/...` path. Select baudrate `460800` and disable
   hardware flow control. With native USB, this baudrate is a host serial
   setting; it does not select a physical UART bit rate on the XIAO.
4. Save and start the app. Check the startup log for successful RCP
   initialization, rather than relying only on a "No errors reported" message.
5. HA normally configures the OTBR integration automatically. Open the Thread
   integration and confirm the intended network and border router are listed.
6. For Matter-over-Thread accessories, also install the Matter integration and
   official Matter Server app. Follow HA's phone/Thread credential instructions
   before scanning the accessory's Matter setup code in the Companion app.

The XIAO itself will not appear as a Matter sensor or switch. A discovered OTBR
service and a Thread network listing do not establish successful accessory
commissioning or radio reliability.

Official host documentation:
[OTBR](https://www.home-assistant.io/integrations/otbr/),
[Thread](https://www.home-assistant.io/integrations/thread/),
[Matter](https://www.home-assistant.io/integrations/matter/).

## Troubleshooting

- **No USB serial device:** use a data cable, try BOOT/RESET download mode,
  and confirm that the OS sees the board. After flashing, USB enumeration
  or device numbering may change; reselect the application-mode device.
- **RCP initialization timeout:** confirm the selected port belongs to the
  XIAO, flow control is disabled, and no serial monitor/other radio service
  owns it. Re-run the build checker to exclude a UART RCP image.
- **Only a REST URL prompt appears:** this is the manual OTBR integration
  dialog. Install/configure the OTBR app first. The USB RCP has no IP address.
- **NVS initialization failure after changing firmware:** the application
  checks initialization errors and does not silently erase NVS. Diagnose the
  error first; a deliberate flash erase also deletes any settings left by the
  previous firmware.
- **Thread network listed but an accessory cannot join:** inspect OTBR/Matter
  logs, phone Thread credentials, pairing mode, local IPv6 connectivity and
  radio placement. A successful firmware flash alone is not a pairing test.

## Provenance and validation

The entry point is adapted from Espressif's
[`ot_rcp` example at v5.5.2](https://github.com/espressif/esp-idf/tree/v5.5.2/examples/openthread/ot_rcp).
Its CC0-1.0 notice is preserved in `main/main.c`. The adaptation removes unused
UART/SPI configuration and example-common components, retaining the SDK's USB
RCP initialization. Other project files follow this repository's GPL-3.0 license.

See [VALIDATION.md](VALIDATION.md) for build results and the distinction between
the original hardware-tested example and this packaged project. This is an
experimental development configuration, not Thread/Matter certification.

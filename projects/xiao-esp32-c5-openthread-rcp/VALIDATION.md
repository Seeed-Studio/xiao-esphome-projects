# Validation

Updated 2026-11-27. Build/configuration checks and hardware/HA observations are
separate evidence classes.

## Source and configuration review

- Source framework: clean ESP-IDF tag `v5.5.2`, including bundled OpenThread.
- Traced `esp_openthread_start()` into the SDK worker and USB host-interface
  initialization. `HOST_CONNECTION_MODE_RCP_USB` installs the USB Serial/JTAG
  driver and registers its file descriptor with OpenThread's main loop.
- The configuration object has static lifetime. NVS, the event loop and the two
  required eventfds are initialized with checked return values.
- No application calls create a Thread network, initialize Wi-Fi, or implement
  a Matter endpoint. Thread network policy belongs to OTBR on the host.
- All text logging is disabled on the USB transport. UART/SPI/CLI roles are
  excluded by defaults and checked against the generated configuration.
- The board's documented 8 MB flash is selected explicitly; the upstream
  example used for the prototype had the generic default of 2 MB.
- The dedicated CI workflow pins `espressif/idf:v5.5.2`; no development SDK,
  preview flags, local SDK paths, or managed-component lockfiles are required.

## Original prototype: hardware and HA observations

The tester compiled Espressif's unmodified v5.5.2 `ot_rcp` example for ESP32-C5,
selected USB RCP in menuconfig, and flashed it to a XIAO ESP32-C5. The flash
output reported a successful written-data hash check.

With the board subsequently attached to the HA OS host and the official OTBR
app configured, screenshots showed:

- An OpenThread Border Router integration without a setup error.
- A preferred Thread network with one Home Assistant OTBR service listed.

These observations support initial host discovery/configuration. They are not
logs proving a specific RCP version exchange or an actual Thread accessory
commissioning test. No Matter-over-Thread accessory or long-run radio test was
recorded. The HA/OTBR versions and controlled USB replug/reboot results were not
captured.

## Packaged project

The sources in this directory simplify the example's entry point to USB-only,
set the correct flash size, and disable bootloader text logging. The packaged
binary has not been flashed or hardware-tested. Build/configuration results
are recorded separately below; prototype observations do not establish that
the packaged binary is hardware-verified.

### Local build result

- Clean `idf.py set-target esp32c5` followed by `idf.py build`: passed with
  ESP-IDF v5.5.2, Python 3.14.5 on macOS, no preview flags.
- Application binary: 241168 bytes; SHA-256
  `0b628bc985c2461b18296f19173ce4ad647ea4f447842647d47737fedbb772a7`.
- Resolved-config checker passed: native USB Radio Only, 8 MB flash, application/
  bootloader/OpenThread log levels zero, no CLI/UART/SPI/FTD/MTD/OTBR roles.
- A negative check using a copy of the generated configuration with USB RCP
  disabled was rejected as expected.
- The generated flash command selects `--flash_size 8MB` and the partition
  size check passed. No firmware was written during this packaging task.

Remote GitHub Actions results must be checked after PR publication.

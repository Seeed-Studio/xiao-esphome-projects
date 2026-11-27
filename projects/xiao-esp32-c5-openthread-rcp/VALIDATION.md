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

## SDK version choice

ESP-IDF v5.5.2 is retained because it is the released SDK used for the original
USB RCP prototype that was flashed and attached to the HA OTBR host. Keeping
that complete SDK/OpenThread release isolates the packaging changes (USB-only
entry point, flash size and silent logging) from an SDK-major migration. This
is not a claim that v6.1.1 is incompatible; that version has not been validated
for this USB RCP project. Aligning with the Zigbee router's v6.1.1 can be a
separate change after the same RCP-initialization and host tests pass.

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
binary was flashed to a XIAO ESP32-C5 on 2026-11-27 and its local USB Spinel
responses and subsequent HA OTBR host startup were checked as described below.
Accessory commissioning and long-run reliability are still unverified.

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
  size check passed. The later bench test used this same packaged application.

## Packaged-binary bench check (2026-11-27)

- Read back and saved the board's complete 8 MB flash before the test. The
  backup contains device-specific data and is not part of this repository.
- Wrote the packaged bootloader, partition table and application using esptool
  v5.7.0. Written-data verification passed for all three images. Application
  SHA-256 matched the build result above; the SDK/application were not rebuilt
  or changed for this flash.
- After reset, a read-only HDLC/Spinel probe over USB at host baudrate 460800,
  without hardware flow control, received valid FCS-checked property replies:
  `PROTOCOL_VERSION = 4.3`, `NCP_VERSION = openthread-esp32/30aaf645242-36b14d3ef;
  esp32c5; 2026-11-27 03:31:01 UTC`, and capabilities
  `[5, 12, 24, 34, 513, 64, 65]`.
- This verifies execution of the packaged image and local USB Spinel property
  exchange. It does not measure over-the-air radio reliability or commission
  an accessory. The probe only read properties; it did not create a dataset.
- Reconnected the packaged-image board to Raspberry Pi 4: HA OS 18.5
  (`aarch64 / raspberrypi4-64`), Core 2026.11.3, Supervisor 2026.11.0;
  official OTBR app 3.4.0, agent `0.3.0-cb4bdae2`, Thread 1.4.0.
- Confirmed the selected Espressif USB Serial/JTAG by-id port, host baudrate
  460800 and hardware flow control disabled. The existing app was stopped
  after the earlier USB removal; explicitly starting it restored service.
- Startup logs showed FCS-decoded Spinel RESET/property responses from this
  adapter, followed by successful `otbr-agent` startup, interface-up and HA
  discovery. At 14:54:30 +08:00 the role changed from detached to leader and
  the Backbone Router became Primary. This verifies packaged RCP/host
  initialization, not accessory commissioning.
- The REST diagnostics emitted transient detached-state warnings before the
  role became leader. No RCP initialization timeout or agent exit was observed
  in this startup check. Controlled host reboot and sustained uptime remain
  untested.

### Sanitized OTBR startup excerpt

Device identifiers, network addresses and dataset details are omitted.

```text
2026-11-27 14:54:15 +08:00 [NOTE]-AGENT---: Running 0.3.0-cb4bdae2
2026-11-27 14:54:15 +08:00 [NOTE]-AGENT---: Thread version: 1.4.0
2026-11-27 14:54:15 +08:00 [N] P-Netif-------: Changing interface state to up.
2026-11-27 14:54:15 +08:00 s6-rc: info: service otbr-agent successfully started
2026-11-27 14:54:16 +08:00 INFO: Successfully sent discovery information to Home Assistant.
2026-11-27 14:54:30 +08:00 [N] Mle-----------: Role detached -> leader
2026-11-27 14:54:30 +08:00 [NOTE]-BBA-----: BackboneAgent: Backbone Router becomes Primary!
```

## Remote CI

Both the shared CI and dedicated USB RCP workflow passed on original PR head
`948e2024964402ed8d78c6d4367e0b208a10c3a9` (runs 45937151545 and
45937151601). Follow-up CI must be checked against the new PR head.

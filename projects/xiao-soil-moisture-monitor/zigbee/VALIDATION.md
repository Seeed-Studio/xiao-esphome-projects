# Validation

Updated 2026-11-26. Build checks, automated host tests, radio behavior, and HA
observations are separate evidence classes. This is an experimental Zigbee
alternative; these results do not establish production battery life.

## Build and host checks

- Repository configuration: ESPHome 2026.11.0, ESP-IDF 5.5.6,
  esp-zigbee-lib 2.0.5, target `seeed_xiao_esp32c6`, 4 MB flash.
- The current repository YAML with factory soil references (dry 2.75 V,
  wet 1.20 V) compiled successfully: config hash `0xd77a23b1`.
- The bench YAML uses measured soil references (dry 2.787 V, wet 2.072 V).
  The final millivolt version compiled, was flashed app-only, and passed
  esptool written-data hash verification: config hash `0xfad9a587`.
  Both configurations use the same C++ component code.
- C++17 host tests pass with `-Wall -Wextra -Werror`: calibration endpoints,
  invalid input/clamping, exact 20%/40% state boundaries, malformed XY reads,
  battery percentage/unknown encodings, and legacy volts-to-millivolts
  conversion across all 71 settings from 1300 to 2000 mV in 10 mV steps.
- The bundled ESPHome 2026.11.0 Zigbee snapshot differs from upstream only
  in `zigbee_helpers_esp32.c`. Its twelve added dispatch lines are recorded
  in `components/zigbee-2026.11.0.patch`; no other upstream code is modified.
- A pinned ESPHome build matrix entry and soil/battery host-test CI job are
  included. Remote CI results must be checked separately after publication.
- Automation example and blueprint YAML were parsed and their entity/input
  wiring checked locally. The new blueprint has not been executed in HA.

## Hardware and HA observations

- Device: XIAO ESP32-C6 Soil Moisture Monitor with external antenna.
- Initial tests: Raspberry Pi 4 / HA OS, Core 2026.11.3, ZHA 2.4.3,
  SONOFF Dongle-LMG21 coordinator. No custom HA quirk or helper was used.
- A later tester screenshot shows the final firmware paired on a new HA
  installation using Connect ZBT-2. Its HA/ZHA versions were not captured.
- App-only updates retained Zigbee pairing on the original installation.
- Measured soil samples included 2.275 V / 71.6% / Normal and
  2.783 V / 0.6% / Dry with the bench references above.
- With the HA refresh automation enabled, the tester confirmed green for wet
  readings and a single-measurement change to red for dry readings. Runtime
  logs observed XY read requests in the same wake cycle. A request is not
  proof of HA receiving its response or rendering the new color.
- Recorded receive windows ended at approximately 4.6-7.9 seconds total
  uptime; a no-read fallback ended at about 9.3 seconds, including sampling
  and startup. Initial commissioning and failed joins take longer.
- Physical indicator LEDs worked after reconnecting USB serial monitoring
  stopped. Reopening the USB monitor caused USB resets and lost the GPIO-wake
  cause used to decide whether to illuminate the indicator.
- HA battery discovery was observed; the tester reported a voltage-estimated
  percentage. A new-installation screenshot shows Battery at 88%.
- The final screenshot confirms Basic identity **XIAO Soil Sensor / Seeed
  Studio** and **Battery Full Voltage = 1,600 mV** without decimal tails.
  It establishes discovery/display, not remote-write persistence.
- On the new HA installation the status-light refresh automation was absent
  and color was stale. Correct entity-specific YAML was provided; successful
  color refresh on that new installation has not yet been confirmed.

## Not verified

- ADC accuracy against a multimeter, battery current, lifetime, or a measured
  discharge curve. Battery percentage is a linear voltage estimate.
- Power-cycle persistence and live migration of an existing volts-based NVS
  setting. Source tracing and host math tests are not substitutes for these.
- A controlled yellow hardware transition, 8-hour sleep/rejoin and parent
  retention, stress/loss testing, or Zigbee2MQTT compatibility.
- The 5-second pairing-reset gesture on hardware. Source inspection confirms
  the reset leave signal is handled by the bundled upstream Zigbee component,
  but this gesture was not part of the bench validation.
- Triple-press calibration: not implemented in this variant.
- The public configuration's factory calibration defaults were build-tested;
  the physical probe was tested with its individually measured references.

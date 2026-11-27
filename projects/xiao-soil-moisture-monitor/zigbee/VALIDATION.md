# Validation

Updated 2026-11-27. Build/static checks, CI, radio behavior and HA observations
are separate evidence classes. This experimental Zigbee configuration does not
establish production battery life.

## Review revision

- ESPHome development revision `5210ad79d95c66155637f5b858088b41f3a2ade4`
  (`2026.12.0-dev`) supplies native Power Configuration support (#21725).
- Removed the frozen Zigbee core snapshot, both hand-assembled APS report
  paths, the global raw observer and the unpublished volts-to-mV migration.
- The read-only color endpoint registers explicit types and access flags through
  the generic cluster/attribute path; it does not require a core routing patch.
- Native attributes use report ENABLE and the common `zigbee.report` action.
  Source tracing: attribute publication schedules a value update; the entity
  loop uses bounded SDK lock acquisition; reporting calls the official
  `ezb_zcl_report_attr_cmd_req`. This is not a delivery acknowledgement.
- Fixed six-second receive window; saved polling intervals are restored before
  sleep. The requested parent timeout is set in the application component with
  a bounded lock, not in YAML.
- CI tracks `dev` temporarily. Return-to-stable condition is documented in
  README and tracked in Seeed PR #78. No local core overlay or release pin.
- Host tests: soil calibration/clamping/invalid references and exact 20%/40%
  thresholds; battery invalid inputs, voltage-estimated percentages and all 71
  full-voltage settings. Native ZCL scaling belongs to upstream tests.
- Mocked application runtime tests pass: lock contention retries, original
  polling-interval restoration (including a restarted window), and all three
  XY color mappings. SDK mocks are not a radio or descriptor integration test.
- Final public YAML compiled with ESP-IDF 6.1.1 / esp-zigbee-lib 2.0.5,
  `seeed_xiao_esp32c6`, 4 MB; config hash `0xecbf2901`. Binary size is
  572816 bytes; flash usage 572455 / 1769472 bytes, RAM 95899 / 452112 bytes.
- The old ESPHome 2026.11.0 CLI rejected the minimum-version requirement as
  expected. The updated native wake-cause helper avoids deprecated app APIs.
- Development-core config validation emits a generic 0 ms schedule warning
  (coerced to 1 ms); the YAML has no zero update intervals. Partition alignment
  reports 124 KB slack. Neither diagnostic prevented a successful build.
- Revised radio/HA behavior: **not flashed or bench-tested**. The historical
  observations below are not proof that native reports, generic color
  descriptors or the fixed receive window work on the revised image.

## Historical hardware and HA observations (pre-review firmware)

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
- Power-cycle persistence of the full-voltage setting. The public revision no
  longer migrates unpublished volts-based bench settings.
- A controlled yellow hardware transition, 8-hour sleep/rejoin and parent
  retention, stress/loss testing, or Zigbee2MQTT compatibility.
- The 5-second pairing-reset gesture on hardware. Source inspection confirms
  the reset leave signal is handled by the upstream Zigbee component,
  but this gesture was not part of the bench validation.
- Triple-press calibration: not implemented in this variant.
- The public configuration's factory calibration defaults were build-tested;
  the physical probe was tested with its individually measured references.

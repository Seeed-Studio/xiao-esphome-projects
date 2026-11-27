# XIAO Soil Moisture Monitor over Zigbee

Optional firmware for the XIAO ESP32-C6 Soil Moisture Monitor. ESPHome builds
the firmware; communication is Zigbee, with no Wi-Fi, API, MQTT or OTA service.
The Zigbee Basic cluster identifies the product as **XIAO Soil Sensor** by
**Seeed Studio**. Use a Zigbee coordinator and ZHA to expose:

- endpoint 1: **Soil Probe Voltage**, in volts, plus standard Power Configuration
  battery percentage and voltage attributes;
- endpoint 2: **Soil Moisture Relative**, a calibrated relative percentage;
- endpoint 3: a logical color light representing Normal (green), Almost
  (yellow), or Dry (red). It does not keep the physical LEDs on;
- endpoint 4: a writable **Battery Full Voltage** number via Analog Output.

## Build and flash

Native Power Configuration support is merged in ESPHome upstream
[#21725](https://github.com/esphome/esphome/pull/21725), but is not in
2026.11.1. Until the stable release includes it, use the official development
version; this is an experimental dependency, not a frozen local core overlay.
CI builds against `dev` to catch upstream API changes. The YAML rejects older
releases rather than silently mixing core versions.

```sh
python3 -m venv .venv
. .venv/bin/activate
pip install 'git+https://github.com/esphome/esphome.git@dev'
esphome compile projects/xiao-soil-moisture-monitor/zigbee/xiao-soil-zigbee.yaml
esphome upload projects/xiao-soil-moisture-monitor/zigbee/xiao-soil-zigbee.yaml --device /dev/ttyACM0
```

Tracked in [Seeed PR #78](https://github.com/Seeed-Studio/xiao-esphome-projects/pull/78):
when a stable ESPHome release includes #21725 and builds this configuration,
change this CI entry back to `stable` and replace the temporary development
install above with `pip install -U esphome`. Do not keep a version pin or copy
ESPHome's Zigbee component into this project. The exact revision used for local
validation is recorded in `VALIDATION.md`.

Enter the XIAO bootloader by holding BOOT, tapping RESET, then releasing BOOT.
After flashing from this mode, tap RESET to boot the application. The board has
4 MB flash. Back up existing firmware before changing connectivity; restoring
an old Wi-Fi image does not migrate its settings into the Zigbee application.

Allow joining in ZHA for initial commissioning. A first join has a bounded
60-second interview window. Short-press the product's GPIO2 measurement button
for a measurement; holding it for 5 seconds resets Zigbee pairing. BOOT is a
different button. Existing installations whose cached interview lacks endpoint 3
or the battery cluster on endpoint 1 need a fresh ZHA interview. App-only
updates preserve flash-stored pairing, but do not force HA to rediscover clusters.

## Calibration and states

Edit `dry_voltage` and `wet_voltage` substitutions using readings from your
probe. Defaults 2.75 V and 1.20 V match the factory ESPHome configuration. They
are reference values, not universal soil parameters. The conversion is:

```text
relative % = clamp((dry_voltage - measured_voltage) /
                   (dry_voltage - wet_voltage) * 100, 0, 100)
```

For example, measured references of 2.787 V dry and 2.072 V wet define a
0.715 V range. Calibration is independent of the state thresholds below.
Use the same insertion depth and soil type for both measurements. Relative
moisture is not volumetric water content. Dry must be greater than wet.
This variant does not implement the factory triple-press calibration gesture.

| State | Relative moisture | Next scheduled measurement |
|---|---|---|
| Dry / red | <= 20% | 15 minutes |
| Almost / yellow | > 20% and <= 40% | 1 hour |
| Normal / green | > 40% | 8 hours |

These thresholds intentionally differ from the factory Wi-Fi YAML's 23%/58%.
Manual measurements briefly show the physical state LED. Timer wakes leave
the LEDs off. GPIO3 low and GPIO14 high select the external antenna: attach
the supplied U.FL antenna. The probe excitation is GPIO21, 200 kHz, 68% duty;
ADC GPIO1; red/yellow/green GPIO20/18/19. Battery ADC is GPIO0.

Battery percentage uses a voltage estimate: 1.2 V = 0%, and the configured
full voltage = 100%, clamped outside the range. The initial full voltage is
1600 mV (1.6 V). The **Battery Full Voltage** number allows 1300-2000 mV
in 10 mV steps. Integer millivolts avoid float32 decimal tails in ZHA;
its last accepted value is saved in device NVS and restored after reboot.
ZHA may retain the previous unit and range until a new interview.
Preferences are synchronized immediately because normal awake windows are short.
Changing the setting recalculates the last ADC sample; it is not a new sample.

Wake the device with a short measurement-button press before changing this
number; writes to a deeply sleeping device may time out. ZHA 2.4.3 supports
this standard Analog Output number without a quirk. The integer-millivolt
control and battery percentage were observed in HA after pairing. Power-cycle
persistence is not yet hardware-verified. Unpublished bench images with
volts-based NVS values are not migrated by this public configuration.
Adding endpoint 4 requires a fresh interview of an already cached device.

GPIO0 is sampled eight times once per measurement. Power Configuration (0x0001)
reports voltage in 100 mV units and percentage in 0.5% units. ZHA creates a
diagnostic Battery entity after discovery; no battery automation is required.
The ADC uses GPIO0 directly with no additional software divider multiplier.
No multimeter comparison or discharge-curve calibration has been performed;
percentage is a linear voltage estimate, not measured remaining capacity.
AA nominal voltage remains 1.5 V in metadata; the configurable full threshold
is a voltage-estimation parameter, not the rated voltage or a chemistry model.

## Updating the status light in ZHA

ZHA 2.4.3 subscribes to on/off and level events for its standard light entity,
but obtains XY color through polling. Color reports alone do not refresh that
entity. Create the automation from `status-light-refresh.yaml`, replacing both
placeholder entity IDs with those in your HA installation. Replace the entire
YAML editor content rather than appending it to an existing automation.
Entity IDs are the `sensor.*` and `light.*` values in each entity's settings,
not the device's display name. The placeholders must be replaced before saving.

Alternatively, copy `blueprints/soil-status-light-refresh.yaml` into
`config/blueprints/automation/seeed/` and reload HA automations. Create an
automation from the blueprint and select the moisture sensor and status light
from the pickers; no manual entity-ID editing is needed.

A new HA installation does not inherit this automation when a device is paired.
Create it again with the new entity IDs, or restore it from the old HA backup.
The trigger runs when the moisture entity changes; an unchanged reading does
not force a refresh. Test with a reading that crosses a color threshold.

After publishing moisture, the firmware requests reports through
`zigbee.report` for every registered report-enabled attribute, including the
color coordinates and native battery sensors. There is no application APS
frame construction, sequence counter, duplicate color packet or global raw
ZCL observer. ESPHome owns attribute writes/report requests and its bounded
lock acquisition. A local enqueue is not a delivery acknowledgement.

For ZHA's color polling, a fixed six-second awake window polls the parent every
100 ms. Both the previous keepalive and fast-poll intervals are saved and
restored before sleep; all application SDK lock attempts are bounded to 10 ms
and retried from the component loop. The fixed window replaces the old
read-observer shortcut. This costs a little more awake time when a read arrives
early and needs new bench validation. It is not a delivery guarantee.

Normal measurement and reporting take several seconds, rather than the
factory Wi-Fi firmware's 120-second awake period. Initial commissioning and
failed network joins take longer. Sleep intervals are configurable; actual
battery life and long-interval parent retention require measurement.

The light is an indicator, not a physical LED control. Commands sent to it
while the device is asleep may time out; measurements reassert its state.
No HA helper, custom quirk or third-party HA integration is required.

The light entity is created automatically after discovery. Its display name is
chosen by ZHA; **Plant Status is an optional local HA rename**, not a name
automatically supplied by this firmware. To use it, open the light settings,
turn off the main-entity naming option if present, and enter `Plant Status`.
Keep its entity ID unchanged so the refresh automation continues to work.
Dashboard cards are also a user configuration, separate from device discovery.

## Implementation boundaries

Battery percentage and voltage use native template/ADC sensors with
`cluster: power_config` and `report: enable`. Native code handles ZCL scaling
and reporting. The application only computes the linear percentage estimate.
No `soil_battery` transport component or copied Zigbee core is included.

`soil_status_light` creates a read-only Color Dimmable Light endpoint (3) for
ZHA, using the core's existing generic cluster descriptors for Level and Color
Control. Explicit ZCL attribute types and READ/REPORT access register these
attributes through the supported generic attribute path. This avoids both
patching the SDK cluster-routing helper and relying on upcoming actuator-light
support. It intentionally does not implement light actuator commands: the
indicator always reflects the measured soil state. ZHA may still display light
controls, but this endpoint is not intended to control a physical lamp.

The small application component also configures a 1024-minute requested parent
aging timeout and the temporary polling window, outside the user YAML. The
parent may impose its own policy; eight-hour sleep/rejoin needs bench testing.

## Validation

```sh
c++ -std=c++17 -Wall -Wextra -Werror \
  projects/xiao-soil-moisture-monitor/zigbee/tests/test_soil_math.cpp \
  -o /tmp/test-soil-math
/tmp/test-soil-math
c++ -std=c++17 -Wall -Wextra -Werror \
  projects/xiao-soil-moisture-monitor/zigbee/tests/test_battery.cpp \
  -o /tmp/test-soil-battery
/tmp/test-soil-battery
python3 projects/xiao-soil-moisture-monitor/zigbee/tests/test_status_runtime.py
```

The host test covers measured calibration samples, clamping, invalid references,
state boundaries and battery clamping/invalid inputs across all full-voltage
settings. CI also builds this configuration against the current official
ESPHome development version until the required native support reaches stable. See `VALIDATION.md` for the
actual hardware and Home Assistant verification status; compilation does not
establish radio delivery or battery life.

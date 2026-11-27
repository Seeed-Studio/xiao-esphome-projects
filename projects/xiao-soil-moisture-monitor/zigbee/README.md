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

Use released ESPHome **2026.11.0** (ESP-IDF 5.5.6, esp-zigbee-lib 2.0.5):

```sh
python3 -m venv .venv
. .venv/bin/activate
pip install esphome==2026.11.0
esphome compile projects/xiao-soil-moisture-monitor/zigbee/xiao-soil-zigbee.yaml
esphome upload projects/xiao-soil-moisture-monitor/zigbee/xiao-soil-zigbee.yaml --device /dev/ttyACM0
```

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
Existing saved settings in volts are converted to millivolts at boot.
ZHA may retain the previous unit and range until a new interview.
Preferences are synchronized immediately because normal awake windows are short.
Changing the setting recalculates the last ADC sample; it is not a new sample.

Wake the device with a short measurement-button press before changing this
number; writes to a deeply sleeping device may time out. ZHA 2.4.3 supports
this standard Analog Output number without a quirk. The integer-millivolt
control and battery percentage were observed in HA after pairing. Power-cycle
persistence and migration of a previously saved volts setting are not yet
hardware-verified.
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

After publishing moisture, this firmware polls its parent every 100 ms while
HA reads on/off, level, then XY. A read observer leaves normal SDK handling
intact and allows a 750 ms response drain after a request for both coordinates.
The receive window is bounded to 6 seconds if the request is absent. Observing
a request or enqueueing a report is **not** an acknowledgement that HA updated.
This is a bounded compromise for a sleepy device, not a delivery guarantee.

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

## Compatibility overlay

`components/zigbee` is the unmodified ESPHome 2026.11.0 Zigbee component **except
for `zigbee_helpers_esp32.c`**, which adds SDK Power Configuration, Level and
Color Control descriptor creation/attribute routing. The full version-matched
snapshot avoids depending
on unreleased ESPHome changes; it is intentionally scoped to this optional
configuration and does not replace the component used by other projects.
Upstream source and GPL-3.0 license:
https://github.com/esphome/esphome/tree/2026.11.0/esphome/components/zigbee

`components/soil_status_light` owns the status reports and receive window. Its
raw ZCL observer uses the SDK's single observer slot and always returns false
so the SDK processes and responds to reads. Do not combine another component
that takes this observer slot without explicit callback composition.

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
```

The host test covers measured calibration samples, clamping, invalid references,
state boundaries and parsing of the XY read request. CI also builds this
configuration with the pinned ESPHome release. See `VALIDATION.md` for the
actual hardware and Home Assistant verification status; compilation does not
establish radio delivery or battery life.

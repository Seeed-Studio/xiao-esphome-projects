#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>

namespace esphome::soil_battery {
// Preserve the former volts-based NVS value while moving the setting to mV.
// The two accepted ranges do not overlap, so migration is idempotent.
inline float normalize_full_millivolts(float saved) {
  if (!std::isfinite(saved)) return 1600.0f;
  if (saved >= 1.3f && saved <= 2.0f) saved *= 1000.0f;
  if (saved < 1300.0f || saved > 2000.0f) return 1600.0f;
  return std::round(saved / 10.0f) * 10.0f;
}
// Zigbee units: voltage is 100 mV, remaining capacity is 0.5 percent.
// 0xff means unknown, not a full battery.
inline uint8_t encode_voltage(float voltage) {
  if (!std::isfinite(voltage) || voltage < 0.0f || voltage > 25.4f) return 0xff;
  return static_cast<uint8_t>(std::lround(voltage * 10.0f));
}
inline uint8_t encode_percentage(float voltage, float empty, float full) {
  if (!std::isfinite(voltage) || voltage < 0.0f || !std::isfinite(empty) ||
      !std::isfinite(full) || empty >= full) return 0xff;
  return static_cast<uint8_t>(std::lround(std::clamp((voltage - empty) / (full - empty), 0.0f, 1.0f) * 200.0f));
}
}  // namespace esphome::soil_battery

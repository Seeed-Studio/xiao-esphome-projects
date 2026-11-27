#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>

namespace esphome::soil_status_light {

enum class SoilState : uint8_t { NORMAL = 1, ALMOST = 2, DRY = 3 };

inline float relative_moisture(float voltage, float dry_voltage, float wet_voltage) {
  if (!std::isfinite(voltage) || !std::isfinite(dry_voltage) || !std::isfinite(wet_voltage) ||
      dry_voltage <= wet_voltage)
    return std::numeric_limits<float>::quiet_NaN();
  return std::clamp((dry_voltage - voltage) / (dry_voltage - wet_voltage) * 100.0f, 0.0f, 100.0f);
}

inline SoilState classify_moisture(float percent) {
  return percent > 40.0f ? SoilState::NORMAL : percent > 20.0f ? SoilState::ALMOST : SoilState::DRY;
}

// Linear voltage estimate; native power_config handles ZCL units/unknowns.
inline float battery_percentage(float voltage, float empty, float full) {
  if (!std::isfinite(voltage) || voltage < 0.0f || !std::isfinite(empty) ||
      !std::isfinite(full) || empty >= full)
    return std::numeric_limits<float>::quiet_NaN();
  return std::clamp((voltage - empty) / (full - empty) * 100.0f, 0.0f, 100.0f);
}

}  // namespace esphome::soil_status_light

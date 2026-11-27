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

// Read Attributes payloads contain little-endian 16-bit IDs. The observer
// must see both coordinates before shortening the receive window.
inline bool requests_xy(const uint8_t *payload, size_t length) {
  if (payload == nullptr || length == 0 || length % 2 != 0) return false;
  bool x = false, y = false;
  for (size_t i = 0; i < length; i += 2) {
    const uint16_t attr = uint16_t(payload[i]) | (uint16_t(payload[i + 1]) << 8);
    x |= attr == 0x0003;
    y |= attr == 0x0004;
  }
  return x && y;
}

}  // namespace esphome::soil_status_light

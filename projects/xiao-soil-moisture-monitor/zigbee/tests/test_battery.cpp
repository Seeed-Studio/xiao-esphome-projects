#include <cassert>
#include <cmath>
#include "../components/soil_status_light/soil_math.h"
using esphome::soil_status_light::battery_percentage;
int main() {
  assert(battery_percentage(1.2f, 1.2f, 1.8f) == 0);
  assert(battery_percentage(1.8f, 1.2f, 1.8f) == 100);
  assert(std::abs(battery_percentage(1.5f, 1.2f, 1.8f) - 50) < 0.001f);
  assert(battery_percentage(1.0f, 1.2f, 1.6f) == 0);
  assert(battery_percentage(1.8f, 1.2f, 1.6f) == 100);
  assert(std::abs(battery_percentage(1.584f, 1.2f, 1.8f) - 64) < 0.001f);
  assert(std::abs(battery_percentage(1.584f, 1.2f, 1.6f) - 96) < 0.001f);
  for (int mv = 1300; mv <= 2000; mv += 10) {
    const float full = mv / 1000.0f;
    assert(battery_percentage(full, 1.2f, full) == 100);
    assert(battery_percentage(1.2f, 1.2f, full) == 0);
    assert(battery_percentage(3.3f, 1.2f, full) == 100);
  }
  assert(std::isnan(battery_percentage(NAN, 1.2f, 1.6f)));
  assert(std::isnan(battery_percentage(-1, 1.2f, 1.6f)));
  assert(std::isnan(battery_percentage(1.4f, 1.6f, 1.2f)));
  assert(std::isnan(battery_percentage(1.4f, 1.2f, 1.2f)));
}

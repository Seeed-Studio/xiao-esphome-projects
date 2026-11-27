#include <cassert>
#include <cmath>
#include "../components/soil_battery/battery_math.h"
using namespace esphome::soil_battery;
int main() {
  assert(normalize_full_millivolts(1.6f) == 1600.0f);
  assert(normalize_full_millivolts(1.62f) == 1620.0f);
  assert(normalize_full_millivolts(1.8f) == 1800.0f);
  assert(normalize_full_millivolts(1.3f) == 1300.0f);
  assert(normalize_full_millivolts(2.0f) == 2000.0f);
  assert(normalize_full_millivolts(1620.0f) == 1620.0f);
  assert(normalize_full_millivolts(NAN) == 1600.0f);
  assert(normalize_full_millivolts(3.0f) == 1600.0f);
  assert(normalize_full_millivolts(2500.0f) == 1600.0f);
  for (int mv = 1300; mv <= 2000; mv += 10) {
    assert(normalize_full_millivolts(mv) == mv);
    assert(normalize_full_millivolts(mv / 1000.0f) == mv);
    assert(encode_percentage(1.584f, 1.2f, mv / 1000.0f) ==
           encode_percentage(1.584f, 1.2f, normalize_full_millivolts(mv / 1000.0f) / 1000.0f));
  }
  constexpr float empty = 1.2f, full = 1.8f;
  assert(encode_percentage(empty, empty, full) == 0);
  assert(encode_percentage(full, empty, full) == 200);
  assert(encode_percentage(1.5f, empty, full) == 100);
  assert(encode_percentage(1.0f, empty, full) == 0);
  assert(encode_percentage(0.0f, empty, full) == 0);
  assert(encode_percentage(1.81f, empty, full) == 200);
  assert(encode_percentage(2.0f, empty, full) == 200);
  assert(encode_percentage(3.3f, empty, full) == 200);
  assert(encode_percentage(NAN, empty, full) == 255);
  assert(encode_percentage(1.4f, full, empty) == 255);
  assert(encode_percentage(1.4f, empty, empty) == 255);
  assert(encode_percentage(-1.0f, empty, full) == 255);
  // Changing the HA full-voltage setting rescales the same sample.
  assert(encode_percentage(1.584f, 1.2f, 1.8f) == 128);  // 64%
  assert(encode_percentage(1.584f, 1.2f, 1.6f) == 192);  // 96%
  assert(encode_percentage(1.6f, 1.2f, 1.6f) == 200);
  assert(encode_percentage(1.8f, 1.2f, 1.6f) == 200);
  assert(encode_voltage(1.2f) == 12);
  assert(encode_voltage(1.5f) == 15);
  assert(encode_voltage(1.8f) == 18);
  assert(encode_voltage(NAN) == 255);
  assert(encode_voltage(26.0f) == 255);
  assert(encode_voltage(-1.0f) == 255);
}

#include <cassert>
#include <cmath>
#include "../components/soil_status_light/soil_math.h"

using namespace esphome::soil_status_light;

int main() {
  constexpr float dry = 2.787f, wet = 2.072f;
  assert(relative_moisture(dry, dry, wet) == 0.0f);
  assert(relative_moisture(wet, dry, wet) == 100.0f);
  assert(std::abs(relative_moisture(2.4295f, dry, wet) - 50.0f) < 0.001f);
  assert(relative_moisture(2.8f, dry, wet) == 0.0f);
  assert(relative_moisture(2.061f, dry, wet) == 100.0f);
  assert(classify_moisture(relative_moisture(2.093f, dry, wet)) == SoilState::NORMAL);
  assert(classify_moisture(relative_moisture(2.55f, dry, wet)) == SoilState::ALMOST);
  assert(classify_moisture(relative_moisture(2.78f, dry, wet)) == SoilState::DRY);
  assert(classify_moisture(40.0f) == SoilState::ALMOST);
  assert(classify_moisture(40.001f) == SoilState::NORMAL);
  assert(classify_moisture(20.0f) == SoilState::DRY);
  assert(classify_moisture(20.001f) == SoilState::ALMOST);
  assert(std::isnan(relative_moisture(NAN, dry, wet)));
  assert(std::isnan(relative_moisture(2.4f, wet, dry)));
  assert(std::isnan(relative_moisture(2.4f, dry, dry)));
}

#pragma once
#include "esphome/core/component.h"
#include "esphome/core/automation.h"
#include "esphome/components/binary_sensor/binary_sensor.h"

namespace esphome {
namespace light_sleep {

class LightSleep : public Component {
 public:
  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::DATA; }
  
  void set_pin_number(int pin) { pin_number_ = pin; }
  void set_wake_up_sensor(binary_sensor::BinarySensor *sensor) { wake_up_sensor_ = sensor; }
  void enter_sleep();

 protected:
  int pin_number_;
  binary_sensor::BinarySensor *wake_up_sensor_{nullptr};
};

template<typename... Ts>
class LightSleepEnterAction : public Action<Ts...> {
 public:
  void set_parent(LightSleep *parent) { parent_ = parent; }
  void play(Ts...) override { parent_->enter_sleep(); }

 protected:
  LightSleep *parent_;
};

}  // namespace light_sleep
}  // namespace esphome
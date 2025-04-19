#pragma once
#include "esphome/core/component.h"
#include "esphome/core/automation.h"
#include <vector>

namespace esphome
{
  namespace light_sleep
  {

    class LightSleep : public Component
    {
    public:
      void setup() override;
      void dump_config() override;
      float get_setup_priority() const override { return setup_priority::DATA; }

      void set_wakeup_pin(int pin) { wakeup_pin_ = pin; }
      void enter_sleep();
      void add_on_wakeup(Trigger<> *trigger) { this->wakeup_triggers_.push_back(trigger); }
      void add_on_prepare_sleep(Trigger<> *trigger) { this->prepare_sleep_triggers_.push_back(trigger); }

    protected:
      int wakeup_pin_;
      std::vector<Trigger<> *> wakeup_triggers_;
      std::vector<Trigger<> *> prepare_sleep_triggers_;
    };

    class LightSleepWakeupTrigger : public Trigger<>, public Parented<LightSleep>
    {
    public:
      explicit LightSleepWakeupTrigger(LightSleep *parent) : Parented(parent)
      {
        parent->add_on_wakeup(this);
      }
    };

    class LightSleepPrepareSleepTrigger : public Trigger<>, public Parented<LightSleep>
    {
    public:
      explicit LightSleepPrepareSleepTrigger(LightSleep *parent) : Parented(parent)
      {
        parent->add_on_prepare_sleep(this);
      }
    };

    template <typename... Ts>
    class LightSleepEnterAction : public Action<Ts...>
    {
    public:
      void set_parent(LightSleep *parent) { parent_ = parent; }
      void play(Ts...) override { parent_->enter_sleep(); }

    protected:
      LightSleep *parent_;
    };

  } // namespace light_sleep
} // namespace esphome
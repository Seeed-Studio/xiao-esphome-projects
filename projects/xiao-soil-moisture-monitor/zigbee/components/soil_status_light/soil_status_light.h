#pragma once
#include <cstdint>
#include "freertos/FreeRTOS.h"
#include "esp_zigbee.h"
#include "esphome/core/component.h"
#include "esphome/components/zigbee/zigbee_attribute_esp32.h"
#include "ezbee/nwk.h"
#include "soil_math.h"

namespace esphome::soil_status_light {
class SoilStatusLight : public Component {
 public:
  void setup() override { this->disable_loop(); }
  void set_zigbee(zigbee::ZigbeeComponent *zb) { this->zb_ = zb; }
  void configure_network() {
    this->configure_pending_ = true;
    this->enable_loop();
  }
  void begin_receive_window() {
    this->poll_pending_ = true;
    this->restore_pending_ = false;
    this->enable_loop();
  }
  void restore_polling() {
    this->poll_pending_ = false;
    this->restore_pending_ = true;
    this->enable_loop();
  }
  bool polling_restored() const { return !this->restore_pending_; }
  void set_attributes(zigbee::ZigbeeAttribute *on, zigbee::ZigbeeAttribute *level,
                      zigbee::ZigbeeAttribute *x, zigbee::ZigbeeAttribute *y) {
    this->on_ = on; this->level_ = level; this->x_ = x; this->y_ = y;
  }
  void publish_status(uint8_t status) {
    // Logical state only. Native zigbee.report sends all registered attributes.
    this->on_->set_attr<bool>(true);
    this->level_->set_attr<uint8_t>(254);
    this->x_->set_attr<uint16_t>(status == 1 ? 11298 : status == 2 ? 29111 : 45914);
    this->y_->set_attr<uint16_t>(status == 1 ? 48942 : status == 2 ? 33849 : 19615);
  }
  void loop() override {
    // SDK configuration stays out of user YAML. Never block the main task
    // indefinitely; retry in a later loop if the Zigbee task holds the lock.
    if (!this->zb_->is_started() || !esp_zigbee_lock_acquire(pdMS_TO_TICKS(10))) return;
    if (this->configure_pending_) {
      ezb_nwk_set_ed_timeout(EZB_NWK_ED_TIMEOUT_1024MIN);
      this->configure_pending_ = false;
    }
    if (this->poll_pending_) {
      if (!this->polling_changed_) {
        this->saved_keepalive_ = ezb_nwk_get_keepalive_interval();
        this->saved_fast_poll_ = ezb_nwk_get_fast_poll_interval();
      }
      ezb_nwk_set_keepalive_interval(100);
      ezb_nwk_set_fast_poll_interval(100);
      this->polling_changed_ = true;
      this->poll_pending_ = false;
    }
    if (this->restore_pending_) {
      if (this->polling_changed_) {
        ezb_nwk_set_keepalive_interval(this->saved_keepalive_);
        ezb_nwk_set_fast_poll_interval(this->saved_fast_poll_);
      }
      this->polling_changed_ = false;
      this->restore_pending_ = false;
    }
    esp_zigbee_lock_release();
    this->disable_loop();
  }
 protected:
  zigbee::ZigbeeComponent *zb_{nullptr};
  zigbee::ZigbeeAttribute *on_{nullptr}, *level_{nullptr}, *x_{nullptr}, *y_{nullptr};
  uint32_t saved_keepalive_{0}, saved_fast_poll_{0};
  bool configure_pending_{false}, poll_pending_{false}, restore_pending_{false}, polling_changed_{false};
};
}  // namespace esphome::soil_status_light

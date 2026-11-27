#pragma once

#include <array>
#include "esphome/core/component.h"
#include "esphome/core/log.h"
#include "esphome/components/zigbee/zigbee_attribute_esp32.h"
#include "ezbee/aps.h"
#include "battery_math.h"

namespace esphome::soil_battery {
class SoilBattery : public Component {
 public:
  void set_zigbee(zigbee::ZigbeeComponent *zb) { zb_ = zb; }
  void set_attributes(zigbee::ZigbeeAttribute *voltage, zigbee::ZigbeeAttribute *percentage) {
    voltage_ = voltage;
    percentage_ = percentage;
  }
  void publish_voltage(float voltage, float empty, float full) {
    voltage_value_ = encode_voltage(voltage);
    percentage_value_ = encode_percentage(voltage, empty, full);
    voltage_->set_attr<uint8_t>(voltage_value_);
    percentage_->set_attr<uint8_t>(percentage_value_);
    ESP_LOGI("soil.battery", "Cell %.3f V, remaining %.1f%% (voltage estimate)", voltage,
             percentage_value_ == 0xff ? NAN : percentage_value_ / 2.0f);
  }
  void report() {
    if (!zb_->is_joined()) return;
    this->cancel_timeout("battery_report_retry");
    // One standard ZCL Report Attributes frame containing both battery values.
    frame_ = {0x18, ++sequence_, 0x0a, 0x20, 0x00, 0x20, voltage_value_,
              0x21, 0x00, 0x20, percentage_value_};
    this->report_attempt_(0);
  }
 protected:
  void retry_(uint8_t attempt) {
    if (attempt < 2) {
      this->set_timeout("battery_report_retry", 100, [this, attempt]() { this->report_attempt_(attempt + 1); });
    } else {
      ESP_LOGW("soil.battery", "Battery report enqueue failed after three attempts");
    }
  }
  void report_attempt_(uint8_t attempt) {
    if (!zb_->is_joined()) return;
    if (!esp_zigbee_lock_acquire(10 / portTICK_PERIOD_MS)) {
      this->retry_(attempt);
      return;
    }
    ezb_apsde_data_req_t req = {};
    req.dst_address.addr_mode = EZB_ADDR_MODE_SHORT;
    req.dst_address.u.short_addr = 0;
    req.src_endpoint = 1;
    req.dst_endpoint = 1;
    req.profile_id = 0x0104;
    req.cluster_id = 0x0001;
    req.tx_options = EZB_APSDE_TX_OPT_ACK_TX;
    req.asdu = frame_.data();
    req.asdu_length = frame_.size();
    const auto result = ezb_apsde_data_request(&req);
    esp_zigbee_lock_release();
    if (result != EZB_ERR_NONE) this->retry_(attempt);
  }
  zigbee::ZigbeeComponent *zb_{nullptr};
  zigbee::ZigbeeAttribute *voltage_{nullptr}, *percentage_{nullptr};
  uint8_t voltage_value_{0xff}, percentage_value_{0xff}, sequence_{0};
  std::array<uint8_t, 11> frame_{};
};
}  // namespace esphome::soil_battery

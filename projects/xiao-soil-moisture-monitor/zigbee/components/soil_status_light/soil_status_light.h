#pragma once
#include <array>
#include <atomic>
#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/core/log.h"
#include "esphome/components/zigbee/zigbee_attribute_esp32.h"
#include "ezbee/aps.h"
#include "ezbee/nwk.h"
#include "ezbee/zcl/zcl_core.h"
#include "soil_math.h"

namespace esphome::soil_status_light {
class SoilStatusLight : public Component {
 public:
  void setup() override { this->disable_loop(); }
  void set_zigbee(zigbee::ZigbeeComponent *zb) { zb_ = zb; }
  void begin_receive_window() {
    // This SDK has a single raw observer slot. No other component in this
    // firmware registers it. Return false to retain normal SDK responses.
    xy_read_ms_.store(0);
    window_start_ms_ = millis();
    if (esp_zigbee_lock_acquire(portMAX_DELAY)) {
      observer_ = this;
      ezb_zcl_raw_command_handler_register(observe_read_);
      ezb_nwk_set_keepalive_interval(100);
      ezb_nwk_set_fast_poll_interval(100);
      esp_zigbee_lock_release();
    }
  }
  bool receive_window_complete() const {
    const uint32_t now = millis();
    const uint32_t read_ms = xy_read_ms_.load();
    // A received request is not a delivery acknowledgement. Leave time for
    // its response and queued reports; bound the whole window if HA is absent.
    return !pending_ && ((read_ms != 0 && now - read_ms >= 750 && now - window_start_ms_ >= 1000) ||
                         now - window_start_ms_ >= 6000);
  }
  bool color_read_observed() const { return xy_read_ms_.load() != 0; }
  void set_attributes(zigbee::ZigbeeAttribute *on, zigbee::ZigbeeAttribute *level,
                      zigbee::ZigbeeAttribute *x, zigbee::ZigbeeAttribute *y) {
    on_ = on; level_ = level; x_ = x; y_ = y;
  }
  void publish_status(uint8_t status) {
    // Logical state only. Never energizes physical indicator GPIOs.
    x_value_ = status == 1 ? 11298 : status == 2 ? 29111 : 45914;
    y_value_ = status == 1 ? 48942 : status == 2 ? 33849 : 19615;
    on_->set_attr<bool>(true);
    level_->set_attr<uint8_t>(254);
    x_->set_attr<uint16_t>(x_value_);
    y_->set_attr<uint16_t>(y_value_);
    ESP_LOGI("soil.light", "Status color=%s, XY=(%u,%u)",
             status == 1 ? "green" : status == 2 ? "yellow" : "red", x_value_, y_value_);
  }
  void report_status() {
    if (!zb_->is_joined()) return;
    // Standard ZCL global Report Attributes (0x0a), server -> client.
    // Carry X/Y/mode together; do not depend on a coordinator's configured
    // reporting timers, which can extend beyond the short awake window.
    frames_[0] = {0x18, ++sequence_, 0x0a, 0x00, 0x00, 0x10, 0x01};
    frames_[1] = {0x18, ++sequence_, 0x0a, 0x00, 0x00, 0x20, 0xfe};
    frames_[2] = {0x18, ++sequence_, 0x0a,
      0x03, 0x00, 0x21, uint8_t(x_value_), uint8_t(x_value_ >> 8),
      0x04, 0x00, 0x21, uint8_t(y_value_), uint8_t(y_value_ >> 8),
      0x08, 0x00, 0x30, 0x01};
    step_ = 0; retries_ = 0; next_send_ms_ = millis(); pending_ = true;
    this->enable_loop();
  }
  void loop() override {
    if (!pending_) { this->disable_loop(); return; }
    if (int32_t(millis() - next_send_ms_) < 0) return;
    if (!zb_->is_joined()) { pending_ = false; this->disable_loop(); return; }
    if (!esp_zigbee_lock_acquire(10 / portTICK_PERIOD_MS)) return;
    const uint8_t index = step_ < 3 ? step_ : 2;
    const uint16_t cluster = index == 0 ? 0x0006 : index == 1 ? 0x0008 : 0x0300;
    ezb_apsde_data_req_t req = {};
    req.dst_address.addr_mode = EZB_ADDR_MODE_SHORT;
    req.dst_address.u.short_addr = 0x0000;
    req.src_endpoint = 3; req.dst_endpoint = 1;
    req.profile_id = 0x0104; req.cluster_id = cluster;
    req.tx_options = EZB_APSDE_TX_OPT_ACK_TX;
    req.asdu = frames_[index].data();
    req.asdu_length = index == 2 ? 17 : 7;
    const ezb_err_t result = ezb_apsde_data_request(&req);
    esp_zigbee_lock_release();
    ESP_LOGD("soil.light", "Report cluster=0x%04x attempt=%u accepted=%s code=%d",
             cluster, retries_ + 1, result == EZB_ERR_NONE ? "YES" : "NO", int(result));
    if (result != EZB_ERR_NONE) {
      if (++retries_ < 3) { next_send_ms_ = millis() + 100; return; }
      ESP_LOGW("soil.light", "Report enqueue failed; next measurement will retry");
      pending_ = false; this->disable_loop(); return;
    }
    retries_ = 0;
    if (++step_ >= 4) {
      // Accepted by the local stack is not proof of HA receipt.
      ESP_LOGI("soil.light", "Status reports queued; XY/mode bundled, color repeated once");
      pending_ = false; this->disable_loop(); return;
    }
    next_send_ms_ = millis() + (step_ == 3 ? 400 : 100);
  }
 protected:
  static bool observe_read_(const ezb_zcl_raw_frame_t *frame) {
    if (observer_ == nullptr || frame == nullptr || frame->header == nullptr) return false;
    const auto &header = *frame->header;
    if (header.dst_ep == 3 && header.profile_id == 0x0104 && header.cluster_id == 0x0300 &&
        (header.fc & 0x0f) == 0 && header.cmd_id == 0x00 &&
        requests_xy(frame->payload, frame->payload_length)) {
      observer_->xy_read_ms_.store(millis());
      ESP_LOGI("soil.light", "Coordinator requested current XY; allowing response drain");
    }
    return false;
  }
  inline static SoilStatusLight *observer_{nullptr};
  std::atomic<uint32_t> xy_read_ms_{0};
  uint32_t window_start_ms_{0};
  zigbee::ZigbeeComponent *zb_{nullptr};
  zigbee::ZigbeeAttribute *on_{nullptr}, *level_{nullptr}, *x_{nullptr}, *y_{nullptr};
  uint16_t x_value_{45914}, y_value_{19615};
  std::array<std::array<uint8_t, 17>, 3> frames_{};
  uint8_t sequence_{0}, step_{0}, retries_{0};
  uint32_t next_send_ms_{0};
  bool pending_{false};
};
}  // namespace esphome::soil_status_light

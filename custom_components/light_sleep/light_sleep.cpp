#include "light_sleep.h"
#include "esphome/core/log.h"
#include <driver/gpio.h>
#include <esp_sleep.h>

namespace esphome {
namespace light_sleep {

static const char *TAG = "light_sleep";

void LightSleep::setup() {
  int gpio_num = pin_number_;
  
  // Manually configure GPIO as input mode
  gpio_config_t config = {
      .pin_bit_mask = 1ULL << gpio_num,
      .mode = GPIO_MODE_INPUT,
      .pull_up_en = GPIO_PULLUP_DISABLE,
      .pull_down_en = GPIO_PULLDOWN_DISABLE,
      .intr_type = GPIO_INTR_DISABLE
  };
  gpio_config(&config);
  
  // Enable GPIO wakeup (triggered by low level)
  gpio_wakeup_enable((gpio_num_t)gpio_num, GPIO_INTR_LOW_LEVEL);
  esp_sleep_enable_gpio_wakeup();
  
  // Wait for GPIO to stabilize in an inactive state (high level) to prevent immediate wakeup
  while (gpio_get_level((gpio_num_t)gpio_num) == 0) {
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

void LightSleep::dump_config() {
  ESP_LOGCONFIG(TAG, "Light Sleep:");
  ESP_LOGCONFIG(TAG, "  Pin: GPIO%d", pin_number_);
  if (wake_up_sensor_ != nullptr) {
    ESP_LOGCONFIG(TAG, "  Wake Up Sensor configured");
  }
}

void LightSleep::enter_sleep() {
 int gpio_num = pin_number_;
    // Wait for GPIO to become high level (button released)
    while (gpio_get_level((gpio_num_t)gpio_num) == 0) {
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    ESP_LOGI(TAG, "Entering light sleep");
    esp_light_sleep_start();
    ESP_LOGI(TAG, "Woke up from light sleep");
    if (wake_up_sensor_ != nullptr) {
        wake_up_sensor_->publish_state(true);
    }
}

}  // namespace light_sleep
}  // namespace esphome
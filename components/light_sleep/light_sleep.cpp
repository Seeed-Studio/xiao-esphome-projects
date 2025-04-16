#include "light_sleep.h"
#include "esphome/core/log.h"
#include <driver/gpio.h>
#include <esp_sleep.h>

namespace esphome {
namespace light_sleep {

static const char *TAG = "light_sleep";

void LightSleep::setup() {
    int gpio_num = wakeup_pin_;
    
    gpio_config_t config = {
        .pin_bit_mask = 1ULL << gpio_num,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&config);
    
    gpio_wakeup_enable((gpio_num_t)gpio_num, GPIO_INTR_LOW_LEVEL);
    esp_sleep_enable_gpio_wakeup();

    vTaskDelay(pdMS_TO_TICKS(100));
    while (gpio_get_level((gpio_num_t)gpio_num) == 0) {
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void LightSleep::dump_config() {
    ESP_LOGCONFIG(TAG, "Light Sleep:");
    ESP_LOGCONFIG(TAG, "  Pin: GPIO%d", wakeup_pin_);
}

void LightSleep::enter_sleep() {
    int gpio_num = wakeup_pin_;
    
    // Wait for the pin to be released (from low to high)
    if (gpio_get_level((gpio_num_t)gpio_num) == 0) {
        ESP_LOGD(TAG, "Waiting for wakeup pin to be released");
        while (gpio_get_level((gpio_num_t)gpio_num) == 0) {
            vTaskDelay(pdMS_TO_TICKS(10));
        }
        vTaskDelay(pdMS_TO_TICKS(50));  // Ensure the pin is stable
    }
    
    ESP_LOGI(TAG, "Entering light sleep");
    esp_light_sleep_start();
    ESP_LOGI(TAG, "Woke up from light sleep");
    
    vTaskDelay(pdMS_TO_TICKS(50));
    for (auto *trigger : this->wakeup_triggers_) {
        trigger->trigger();
    }
}

}  // namespace light_sleep
}  // namespace esphome
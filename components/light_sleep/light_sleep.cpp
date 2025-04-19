#include "light_sleep.h"
#include "esphome/core/log.h"
#include "esphome/core/application.h"
#include <driver/gpio.h>
#include <esp_sleep.h>
#include <esp_wifi.h>

namespace esphome
{
    namespace light_sleep
    {

        static const char *TAG = "light_sleep";

        void LightSleep::setup()
        {
            int gpio_num = wakeup_pin_;

            gpio_config_t config = {
                .pin_bit_mask = 1ULL << gpio_num,
                .mode = GPIO_MODE_INPUT,
                .pull_up_en = GPIO_PULLUP_ENABLE,
                .pull_down_en = GPIO_PULLDOWN_DISABLE,
                .intr_type = GPIO_INTR_DISABLE};
            gpio_config(&config);

            gpio_wakeup_enable((gpio_num_t)gpio_num, GPIO_INTR_LOW_LEVEL);
            esp_sleep_enable_gpio_wakeup();

            vTaskDelay(pdMS_TO_TICKS(100));
            if (gpio_get_level((gpio_num_t)gpio_num) == 0)
            {
                ESP_LOGW(TAG, "Wakeup pin is low during setup, waiting for release");
                while (gpio_get_level((gpio_num_t)gpio_num) == 0)
                {
                    vTaskDelay(pdMS_TO_TICKS(10));
                }
            }
            ESP_LOGI(TAG, "Light sleep setup complete, wakeup pin: GPIO%d", gpio_num);
        }

        void LightSleep::dump_config()
        {
            ESP_LOGCONFIG(TAG, "Light Sleep:");
            ESP_LOGCONFIG(TAG, "  Pin: GPIO%d", wakeup_pin_);
        }

        void LightSleep::enter_sleep()
        {
            int gpio_num = wakeup_pin_;

            // Wait for the pin to be released (from low to high)
            if (gpio_get_level((gpio_num_t)gpio_num) == 0)
            {
                ESP_LOGD(TAG, "Waiting for wakeup pin to be released");
                while (gpio_get_level((gpio_num_t)gpio_num) == 0)
                {
                    vTaskDelay(pdMS_TO_TICKS(10));
                }
            }

            vTaskDelay(pdMS_TO_TICKS(50)); // Ensure the pin is stable

            // 触发用户定义的睡眠前动作
            ESP_LOGI(TAG, "Triggering prepare sleep actions");
            for (auto *trigger : this->prepare_sleep_triggers_)
            {
                trigger->trigger();
            }

            // 通知 HA 设备即将进入睡眠
            ESP_LOGI(TAG, "Notifying Home Assistant of shutdown");
            App.run_safe_shutdown_hooks();

            vTaskDelay(pdMS_TO_TICKS(800));

            // 禁用 WiFi
            ESP_LOGI(TAG, "Disabling WiFi");
            if (esp_wifi_stop() != ESP_OK)
            {
                ESP_LOGE(TAG, "Failed to disable WiFi");
            }

            // 进入轻度睡眠
            ESP_LOGI(TAG, "Entering light sleep");
            esp_light_sleep_start();

            esp_sleep_enable_gpio_wakeup(); // 再次启用

            // 唤醒后重新启用 WiFi
            ESP_LOGI(TAG, "Woke up from light sleep, enabling WiFi");
            if (esp_wifi_start() != ESP_OK)
            {
                ESP_LOGE(TAG, "Failed to enable WiFi");
            }

            // 等待 WiFi 稳定
            vTaskDelay(pdMS_TO_TICKS(100));

            // 触发唤醒动作
            for (auto *trigger : this->wakeup_triggers_)
            {
                trigger->trigger();
            }
        }

    } // namespace light_sleep
} // namespace esphome
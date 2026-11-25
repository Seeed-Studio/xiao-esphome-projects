#include "esp_err.h"
#include "esp_log.h"
#include "esp_system.h"
#include <inttypes.h>
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_zigbee_core.h"
#include "esp_zigbee_endpoint.h"
#include "esp_zigbee_cluster.h"
#include "esp_zigbee_attribute.h"
#include "zcl/esp_zigbee_zcl_core.h"
#include "zdo/esp_zigbee_zdo_command.h"
#include "zboss_api.h"

#define TAG "C5_ZB_ROUTER"
#define ZB_CHANNEL_MASK ESP_ZB_TRANSCEIVER_ALL_CHANNELS_MASK
#define ZB_MAX_CHILDREN 10
#define STEERING_RETRY_MS 10000
#define BOOT_BUTTON_GPIO GPIO_NUM_28
#define BUTTON_DEBOUNCE_MS 40
#define BUTTON_LONG_PRESS_MS 8000
#define ROUTER_ENDPOINT 1
#define ZB_MANUFACTURER_NAME "\x0c" "Seeed Studio"
#define ZB_MODEL_IDENTIFIER "\x0d" "XIAO ESP32-C5"

static void retry_network_steering(uint8_t mode);
static volatile bool s_network_joined;

static void lqi_response_cb(const esp_zb_zdo_mgmt_lqi_rsp_t *rsp, void *ctx)
{
    (void)ctx;
    if (rsp && rsp->status == 0) {
        ESP_LOGI(TAG, "Coordinator neighbor table response: %u entries; inspect ZHA device info",
                 rsp->neighbor_table_entries);
    } else {
        ESP_LOGW(TAG, "Coordinator LQI-table request failed (status=%u)", rsp ? rsp->status : 0xff);
    }
}

static void refresh_link_reading(void)
{
    if (!s_network_joined) {
        ESP_LOGW(TAG, "BOOT short press: not joined; starting network steering");
        esp_zb_bdb_start_top_level_commissioning(ESP_ZB_BDB_MODE_NETWORK_STEERING);
        return;
    }
    esp_zb_zdo_mgmt_lqi_req_param_t req = {
        .start_index = 0,
        .dst_addr = 0x0000,
    };
    esp_zb_zdo_mgmt_lqi_req(&req, lqi_response_cb, NULL);
    ESP_LOGI(TAG, "BOOT short press: requested coordinator neighbor/LQI table; outgoing Zigbee traffic prompts fresh ZHA link metrics");
}

static void identify_button_cb(uint8_t identify_on)
{
    if (identify_on) {
        ESP_LOGI(TAG, "HA Identify diagnostic button pressed; refreshing Zigbee link reading");
        refresh_link_reading();
    }
}

static void boot_button_task(void *arg)
{
    (void)arg;
    bool was_pressed = false;
    TickType_t pressed_at = 0;
    bool long_action_done = false;
    while (true) {
        bool pressed = gpio_get_level(BOOT_BUTTON_GPIO) == 0;
        if (pressed && !was_pressed) {
            vTaskDelay(pdMS_TO_TICKS(BUTTON_DEBOUNCE_MS));
            pressed = gpio_get_level(BOOT_BUTTON_GPIO) == 0;
            if (pressed) {
                pressed_at = xTaskGetTickCount();
                long_action_done = false;
            }
        }
        if (pressed && was_pressed && !long_action_done &&
            (xTaskGetTickCount() - pressed_at) >= pdMS_TO_TICKS(BUTTON_LONG_PRESS_MS)) {
            long_action_done = true;
            if (esp_zb_lock_acquire(portMAX_DELAY)) {
                ESP_LOGW(TAG, "BOOT held >= 8s: Zigbee local reset (leave network, clear settings)");
                /* No-op while not on a network; the ESP_ZB_ZDO_SIGNAL_LEAVE handler below
                 * reboots once the reset has completed. */
                esp_zb_bdb_reset_via_local_action();
                esp_zb_lock_release();
            }
        }
        if (!pressed && was_pressed && !long_action_done) {
            if (esp_zb_lock_acquire(portMAX_DELAY)) {
                refresh_link_reading();
                esp_zb_lock_release();
            }
        }
        was_pressed = pressed;
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

void esp_zb_app_signal_handler(esp_zb_app_signal_t *signal)
{
    uint32_t sig = *signal->p_app_signal;
    switch (sig) {
    case ESP_ZB_ZDO_SIGNAL_SKIP_STARTUP:
        ESP_LOGI(TAG, "Starting Zigbee Router stack");
        esp_zb_bdb_start_top_level_commissioning(ESP_ZB_BDB_MODE_INITIALIZATION);
        break;
    case ESP_ZB_BDB_SIGNAL_DEVICE_FIRST_START:
    case ESP_ZB_BDB_SIGNAL_DEVICE_REBOOT:
        if (signal->esp_err_status == ESP_OK) {
            s_network_joined = !esp_zb_bdb_is_factory_new();
            ESP_LOGI(TAG, "Router initialized; factory-new=%d", esp_zb_bdb_is_factory_new());
            if (esp_zb_bdb_is_factory_new()) {
                esp_zb_bdb_start_top_level_commissioning(ESP_ZB_BDB_MODE_NETWORK_STEERING);
            }
        } else {
            ESP_LOGE(TAG, "Zigbee init failed: %s", esp_err_to_name(signal->esp_err_status));
        }
        break;
    case ESP_ZB_BDB_SIGNAL_STEERING:
        if (signal->esp_err_status == ESP_OK) {
            s_network_joined = true;
            ESP_LOGI(TAG, "Joined Zigbee network successfully");
        } else {
            s_network_joined = false;
            ESP_LOGW(TAG, "Network steering failed (%s); retry in %u ms",
                     esp_err_to_name(signal->esp_err_status), STEERING_RETRY_MS);
            esp_zb_scheduler_alarm(retry_network_steering,
                                   ESP_ZB_BDB_MODE_NETWORK_STEERING, STEERING_RETRY_MS);
        }
        break;
    case ESP_ZB_ZDO_SIGNAL_LEAVE: {
        /* Raised once the device is off the network. A RESET-type leave (local reset, or a
         * removal by the coordinator) has also wiped the Zigbee persistent data, but the stack
         * neither reboots nor starts commissioning by itself, so the router would sit offline
         * until power-cycled. Reboot instead: the first-start path above then sees a factory-new
         * device and starts network steering (it retries every STEERING_RETRY_MS). */
        esp_zb_zdo_signal_leave_params_t *leave =
            (esp_zb_zdo_signal_leave_params_t *)esp_zb_app_signal_get_params(signal->p_app_signal);
        s_network_joined = false;
        if (leave->leave_type == ESP_ZB_NWK_LEAVE_TYPE_RESET) {
            ESP_LOGW(TAG, "Left network and cleared Zigbee settings; restarting to steer again");
            esp_restart();
        }
        ESP_LOGW(TAG, "Left network (leave_type=%u)", leave->leave_type);
        break;
    }
    default:
        ESP_LOGI(TAG, "Zigbee signal=%" PRIu32 " status=%s", sig, esp_err_to_name(signal->esp_err_status));
        break;
    }
}

static void retry_network_steering(uint8_t mode)
{
    esp_err_t err = esp_zb_bdb_start_top_level_commissioning(mode);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "Unable to schedule network steering: %s", esp_err_to_name(err));
        esp_zb_scheduler_alarm(retry_network_steering, mode, STEERING_RETRY_MS);
    }
}

static void zigbee_task(void *arg)
{
    (void)arg;
    esp_zb_cfg_t cfg = {
        .esp_zb_role = ESP_ZB_DEVICE_TYPE_ROUTER,
        .install_code_policy = false,
        .nwk_cfg.zczr_cfg.max_children = ZB_MAX_CHILDREN,
    };
    esp_zb_init(&cfg);
    esp_zb_set_primary_network_channel_set(ZB_CHANNEL_MASK);

    esp_zb_ep_list_t *ep_list = esp_zb_ep_list_create();
    esp_zb_cluster_list_t *cluster_list = esp_zb_zcl_cluster_list_create();
    ESP_ERROR_CHECK(ep_list && cluster_list ? ESP_OK : ESP_ERR_NO_MEM);
    esp_zb_endpoint_config_t endpoint_config = {
        .endpoint = ROUTER_ENDPOINT,
        .app_profile_id = ESP_ZB_AF_HA_PROFILE_ID,
        .app_device_id = ESP_ZB_HA_RANGE_EXTENDER_DEVICE_ID,
        .app_device_version = 0,
    };
    esp_zb_attribute_list_t *basic_cluster = esp_zb_basic_cluster_create(NULL);
    ESP_ERROR_CHECK(basic_cluster ? ESP_OK : ESP_ERR_NO_MEM);
    ESP_ERROR_CHECK(esp_zb_basic_cluster_add_attr(
        basic_cluster, ESP_ZB_ZCL_ATTR_BASIC_MANUFACTURER_NAME_ID, ZB_MANUFACTURER_NAME));
    ESP_ERROR_CHECK(esp_zb_basic_cluster_add_attr(
        basic_cluster, ESP_ZB_ZCL_ATTR_BASIC_MODEL_IDENTIFIER_ID, ZB_MODEL_IDENTIFIER));
    ESP_ERROR_CHECK(esp_zb_cluster_list_add_basic_cluster(
        cluster_list, basic_cluster, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE));
    ESP_ERROR_CHECK(esp_zb_cluster_list_add_identify_cluster(
        cluster_list, esp_zb_identify_cluster_create(NULL), ESP_ZB_ZCL_CLUSTER_SERVER_ROLE));
    ESP_ERROR_CHECK(esp_zb_ep_list_add_ep(ep_list, cluster_list, endpoint_config));
    ESP_ERROR_CHECK(esp_zb_device_register(ep_list));
    esp_zb_identify_notify_handler_register(ROUTER_ENDPOINT, identify_button_cb);

    ESP_ERROR_CHECK(esp_zb_start(false));
    esp_zb_stack_main_loop();
}

void app_main(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    ESP_ERROR_CHECK(gpio_config(&(gpio_config_t){
        .pin_bit_mask = 1ULL << BOOT_BUTTON_GPIO,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    }));
    esp_zb_platform_config_t platform = {
        .radio_config.radio_mode = ZB_RADIO_MODE_NATIVE,
        .host_config.host_connection_mode = ZB_HOST_CONNECTION_MODE_NONE,
    };
    ESP_ERROR_CHECK(esp_zb_platform_config(&platform));
    xTaskCreate(boot_button_task, "boot_button", 3072, NULL, 4, NULL);
    xTaskCreate(zigbee_task, "zigbee", 6144, NULL, 5, NULL);
}

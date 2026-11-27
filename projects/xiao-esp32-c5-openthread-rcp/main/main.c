/*
 * SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
 * SPDX-License-Identifier: CC0-1.0
 *
 * Adapted from ESP-IDF v5.5.2 examples/openthread/ot_rcp for the
 * Seeed Studio XIAO ESP32-C5 native USB Serial/JTAG connection.
 */

#include "sdkconfig.h"
#include "driver/usb_serial_jtag.h"
#include "esp_err.h"
#include "esp_event.h"
#include "esp_openthread.h"
#include "esp_vfs_eventfd.h"
#include "nvs_flash.h"

#if !CONFIG_IDF_TARGET_ESP32C5 || !CONFIG_OPENTHREAD_RADIO || !CONFIG_OPENTHREAD_RCP_USB_SERIAL_JTAG
#error "This project requires ESP32-C5 in OpenThread Radio Only / USB RCP mode"
#endif

void app_main(void)
{
    /* One eventfd for the OpenThread task queue and one for the radio driver. */
    const esp_vfs_eventfd_config_t eventfd_config = {
        .max_fds = 2,
    };
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(esp_vfs_eventfd_register(&eventfd_config));

    /* Static lifetime: the SDK starts its OpenThread worker using this config. */
    static esp_openthread_config_t config = {
        .netif_config = {0},
        .platform_config = {
            .radio_config = {
                .radio_mode = RADIO_MODE_NATIVE,
            },
            .host_config = {
                .host_connection_mode = HOST_CONNECTION_MODE_RCP_USB,
                .host_usb_config = USB_SERIAL_JTAG_DRIVER_CONFIG_DEFAULT(),
            },
            .port_config = {
                .storage_partition_name = "nvs",
                .netif_queue_size = 10,
                .task_queue_size = 10,
            },
        },
    };

    /* The SDK initializes Spinel and runs the radio task. No Thread dataset,
     * Wi-Fi network or Matter application is created by this firmware. */
    ESP_ERROR_CHECK(esp_openthread_start(&config));
}

#pragma once

#include "soc/soc_caps.h"
#include "sdkconfig.h"
#if SOC_IEEE802154_SUPPORTED && CONFIG_ZB_ENABLED

#include "ZigbeeEP.h"
#include "ha/esp_zigbee_ha_standard.h"
#include "zcl/esp_zigbee_zcl_power_config.h"

// Default configuration for binary sensor device
#define ZIGBEE_DEFAULT_BINARY_CONFIG()                                                             \
    {                                                                                              \
        .basic_cfg =                                                                               \
            {                                                                                      \
                .zcl_version = ESP_ZB_ZCL_BASIC_ZCL_VERSION_DEFAULT_VALUE,                         \
                .power_source = ESP_ZB_ZCL_BASIC_POWER_SOURCE_DEFAULT_VALUE,                       \
            },                                                                                     \
        .identify_cfg =                                                                            \
            {                                                                                      \
                .identify_time = ESP_ZB_ZCL_IDENTIFY_IDENTIFY_TIME_DEFAULT_VALUE,                  \
            },                                                                                     \
        .ias_zone_cfg =                                                                            \
            {                                                                                      \
                .zone_state = ESP_ZB_ZCL_IAS_ZONE_ZONESTATE_NOT_ENROLLED,                          \
                .zone_type = ESP_ZB_ZCL_IAS_ZONE_ZONETYPE_CONTACT_SWITCH,                          \
                .zone_status = 0,                                                                  \
                .ias_cie_addr = ESP_ZB_ZCL_ZONE_IAS_CIE_ADDR_DEFAULT,                              \
                .zone_id = 0xff,                                                                   \
                .zone_ctx = {0, 0, 0, 0},                                                          \
            },                                                                                     \
        .power_config_cfg =                                                                        \
        {                                                                                          \
            .main_voltage = 0,                                                                     \
            .main_freq = 0,                                                                        \
            .main_alarm_mask = ESP_ZB_ZCL_POWER_CONFIG_MAINS_ALARM_MASK_DEFAULT_VALUE,             \
            .main_voltage_min = ESP_ZB_ZCL_POWER_CONFIG_MAINS_VOLTAGE_MIN_THRESHOLD_DEFAULT_VALUE, \
            .main_voltage_max = ESP_ZB_ZCL_POWER_CONFIG_MAINS_VOLTAGE_MAX_THRESHOLD_DEFAULT_VALUE, \
            .main_voltage_dwell = ESP_ZB_ZCL_POWER_CONFIG_MAINS_DWELL_TRIP_POINT_DEFAULT_VALUE,    \
        }                                                                                          \
    }

typedef struct zigbee_binary_sensor_cfg_s
{
    esp_zb_basic_cluster_cfg_t basic_cfg;
    esp_zb_identify_cluster_cfg_t identify_cfg;
    esp_zb_ias_zone_cluster_cfg_t ias_zone_cfg;
    esp_zb_power_config_cluster_cfg_t power_config_cfg;
} zigbee_binary_sensor_cfg_t;

class ZigbeeBinarySensor : public ZigbeeEP
{
public:
    /**
     * @brief Constructor for ZigbeeBinarySensor
     * @param endpoint_id Endpoint ID for this device
     */
    ZigbeeBinarySensor(uint8_t endpoint_id);
    ~ZigbeeBinarySensor() {}

    void setStatus(bool status);

private:
    void report();
    void zbIASZoneEnrollResponse(const esp_zb_zcl_ias_zone_enroll_response_message_t *message) override;
    uint8_t _zone_status;
    uint8_t _zone_id;
    esp_zb_ieee_addr_t _ias_cie_addr;
    uint8_t _ias_cie_endpoint;
};

#endif // SOC_IEEE802154_SUPPORTED && CONFIG_ZB_ENABLED
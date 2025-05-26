import esphome.codegen as cg
from esphome.components import text_sensor
import esphome.config_validation as cv
from esphome.const import (
    CONF_STATUS,
    CONF_MAC_ADDRESS,
    CONF_VERSION,
    ENTITY_CATEGORY_DIAGNOSTIC,
    ICON_MOTION_SENSOR,
    ICON_BLUETOOTH,
    ICON_CHIP,
)

from . import CONF_LD2410_ID, LD2410Component
from esphome.components.deep_sleep import DeepSleepComponent
from esphome.components import switch

CONF_DEEP_SLEEP_ID = "deep_sleep_id"
CONF_LED_ID = "led_id"

DEPENDENCIES = ["ld2410", "deep_sleep"]

CONFIG_SCHEMA = {
    cv.GenerateID(CONF_LD2410_ID): cv.use_id(LD2410Component),
    cv.Optional(CONF_VERSION): text_sensor.text_sensor_schema(
        entity_category=ENTITY_CATEGORY_DIAGNOSTIC, icon=ICON_CHIP
    ),
    cv.Optional(CONF_MAC_ADDRESS): text_sensor.text_sensor_schema(
        entity_category=ENTITY_CATEGORY_DIAGNOSTIC, icon=ICON_BLUETOOTH
    ),
    cv.Optional(CONF_STATUS): text_sensor.text_sensor_schema(
        icon=ICON_MOTION_SENSOR
    ).extend({
        cv.Required(CONF_DEEP_SLEEP_ID): cv.use_id(DeepSleepComponent),
        cv.Optional(CONF_LED_ID): cv.use_id(switch.Switch)
    }),
}


async def to_code(config):
    ld2410_component = await cg.get_variable(config[CONF_LD2410_ID])
    if version_config := config.get(CONF_VERSION):
        sens = await text_sensor.new_text_sensor(version_config)
        cg.add(ld2410_component.set_version_text_sensor(sens))
    if mac_address_config := config.get(CONF_MAC_ADDRESS):
        sens = await text_sensor.new_text_sensor(mac_address_config)
        cg.add(ld2410_component.set_mac_text_sensor(sens))
    if status_config := config.get(CONF_STATUS):
        sens = await text_sensor.new_text_sensor(status_config)
        cg.add(ld2410_component.set_status_text_sensor(sens))
        deep_sleep_component = await cg.get_variable(config[CONF_STATUS][CONF_DEEP_SLEEP_ID])
        cg.add(ld2410_component.set_deep_sleep(deep_sleep_component))
        if led_config := status_config.get(CONF_LED_ID):
            switch_component = await cg.get_variable(led_config)
            cg.add(ld2410_component.set_led_switch(switch_component))

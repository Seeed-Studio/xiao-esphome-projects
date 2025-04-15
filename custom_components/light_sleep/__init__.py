import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import binary_sensor
from esphome import automation
from esphome.const import CONF_ID, CONF_PIN

light_sleep_ns = cg.esphome_ns.namespace('light_sleep')
LightSleep = light_sleep_ns.class_('LightSleep', cg.Component)

CONF_WAKE_UP_SENSOR = 'wake_up_sensor'

LightSleepEnterAction = light_sleep_ns.class_('LightSleepEnterAction', automation.Action)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(LightSleep),
    cv.Required(CONF_PIN): cv.int_range(0, 22),  # ESP32-C6 has GPIO0 to GPIO21
    cv.Optional(CONF_WAKE_UP_SENSOR): binary_sensor.binary_sensor_schema({
        cv.Optional('name', default="Light Sleep Wake Up"): cv.string
    }),
}).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    cg.add(var.set_pin_number(config[CONF_PIN]))
    if CONF_WAKE_UP_SENSOR in config:
        sens = await binary_sensor.new_binary_sensor(config[CONF_WAKE_UP_SENSOR])
        cg.add(var.set_wake_up_sensor(sens))

LIGHT_SLEEP_ENTER_SCHEMA = cv.Schema({
    cv.Required(CONF_ID): cv.use_id(LightSleep)
})

@automation.register_action('light_sleep.enter', LightSleepEnterAction, LIGHT_SLEEP_ENTER_SCHEMA)
async def light_sleep_enter_to_code(config, action_id, template_arg, args):
    var = cg.new_Pvariable(action_id, template_arg)
    parent = await cg.get_variable(config[CONF_ID])
    cg.add(var.set_parent(parent))
    return var
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import automation
from esphome.const import CONF_ID, CONF_TRIGGER_ID, CONF_WAKEUP_PIN
from esphome import pins

light_sleep_ns = cg.esphome_ns.namespace('light_sleep')
LightSleep = light_sleep_ns.class_('LightSleep', cg.Component)
LightSleepEnterAction = light_sleep_ns.class_('LightSleepEnterAction', automation.Action)
LightSleepWakeupTrigger = light_sleep_ns.class_('LightSleepWakeupTrigger', automation.Trigger.template(), cg.Parented.template(LightSleep))
LightSleepPrepareSleepTrigger = light_sleep_ns.class_('LightSleepPrepareSleepTrigger', automation.Trigger.template(), cg.Parented.template(LightSleep))

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(LightSleep),
    cv.Required(CONF_WAKEUP_PIN): pins.internal_gpio_input_pin_schema,  # ESP32-C6 has GPIO0 to GPIO21
    cv.Optional('on_wakeup'): automation.validate_automation({
        cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(LightSleepWakeupTrigger),
    }),
    cv.Optional('on_prepare_sleep'): automation.validate_automation({
        cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(LightSleepPrepareSleepTrigger),
    }),
}).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    pin = await cg.gpio_pin_expression(config[CONF_WAKEUP_PIN])
    cg.add(var.set_wakeup_pin(pin))
    
    if 'on_wakeup' in config:
        for conf in config['on_wakeup']:
            trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)  # Pass the parent component through the constructor
            await automation.build_automation(trigger, [], conf)
    
    if 'on_prepare_sleep' in config:
        for conf in config['on_prepare_sleep']:
            trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
            await automation.build_automation(trigger, [], conf)

LIGHT_SLEEP_ENTER_SCHEMA = cv.Schema({
    cv.Required(CONF_ID): cv.use_id(LightSleep)
})

# Register the light_sleep.enter action
@automation.register_action('light_sleep.enter', LightSleepEnterAction, LIGHT_SLEEP_ENTER_SCHEMA)
async def light_sleep_enter_to_code(config, action_id, template_arg, args):
    var = cg.new_Pvariable(action_id, template_arg)
    parent = await cg.get_variable(config[CONF_ID])
    cg.add(var.set_parent(parent))
    return var

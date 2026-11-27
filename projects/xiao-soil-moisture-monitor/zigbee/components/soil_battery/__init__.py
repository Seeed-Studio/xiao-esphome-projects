"""Standard Power Configuration battery attributes for the Soil Monitor."""
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID
from esphome.core import coroutine_with_priority
from esphome.components.zigbee.const import ZigbeeAttribute, ZigbeeComponent

DEPENDENCIES = ["zigbee", "esp32"]
ns = cg.esphome_ns.namespace("soil_battery")
SoilBattery = ns.class_("SoilBattery", cg.Component)
CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(SoilBattery),
    cv.Required("zigbee_id"): cv.use_id(ZigbeeComponent),
    cv.GenerateID("voltage_attribute"): cv.declare_id(ZigbeeAttribute),
    cv.GenerateID("percentage_attribute"): cv.declare_id(ZigbeeAttribute),
}).extend(cv.COMPONENT_SCHEMA)


@coroutine_with_priority(-101.0)
async def to_code(config):
    zb = await cg.get_variable(config["zigbee_id"])
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    cg.add(var.set_zigbee(zb))
    # Reuse endpoint 1; existing moisture and light endpoint identities remain.
    cg.add(zb.add_cluster(1, 0x0001, cg.RawExpression("EZB_ZCL_CLUSTER_SERVER")))
    attrs = []
    for key, attr_id in (("voltage_attribute", 0x0020), ("percentage_attribute", 0x0021)):
        attr = cg.new_Pvariable(config[key], zb, 1, 0x0001,
            cg.RawExpression("EZB_ZCL_CLUSTER_SERVER"), attr_id,
            cg.RawExpression("EZB_ZCL_ATTR_TYPE_UINT8"), 1, 0)
        await cg.register_component(attr, {})
        cg.add(attr.add_attr(cg.TemplateArguments(cg.uint8), 0xff))
        attrs.append(attr)
    cg.add(var.set_attributes(*attrs))
    # AA (ZCL size 3), one cell, 1.5 V rated voltage.
    for attr_id, value in ((0x0031, 3), (0x0033, 1), (0x0034, 15)):
        cg.add(zb.add_attr(cg.TemplateArguments(cg.uint8), 1, 0x0001,
            cg.RawExpression("EZB_ZCL_CLUSTER_SERVER"), attr_id, 0, value))

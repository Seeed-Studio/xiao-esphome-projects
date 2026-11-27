"""Native ZHA color-dimmable endpoint displaying device-computed soil status."""
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID
from esphome.core import coroutine_with_priority
from esphome.components.zigbee.const import ZigbeeAttribute, ZigbeeComponent

DEPENDENCIES = ["zigbee", "esp32"]
ns = cg.esphome_ns.namespace("soil_status_light")
SoilStatusLight = ns.class_("SoilStatusLight", cg.Component)
CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(SoilStatusLight),
    cv.Required("zigbee_id"): cv.use_id(ZigbeeComponent),
    **{cv.GenerateID(k): cv.declare_id(ZigbeeAttribute) for k in
       ("on_attribute", "level_attribute", "x_attribute", "y_attribute")},
}).extend(cv.COMPONENT_SCHEMA)


@coroutine_with_priority(-100.0)
async def to_code(config):
    zb = await cg.get_variable(config["zigbee_id"])
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    cg.add(var.set_zigbee(zb))
    # ZHA Color Dimmable Light device type. Existing sensors stay on endpoints 1/2.
    cg.add(zb.create_default_cluster(3, 0x0102))
    for cluster in (0x0006, 0x0008, 0x0300):
        cg.add(zb.add_cluster(3, cluster, cg.RawExpression("EZB_ZCL_CLUSTER_SERVER")))
    attrs = []
    for key, cluster, attr_id, ztype, ctype, initial in (
        ("on_attribute", 0x0006, 0x0000, "BOOL", cg.bool_, True),
        ("level_attribute", 0x0008, 0x0000, "UINT8", cg.uint8, 254),
        ("x_attribute", 0x0300, 0x0003, "UINT16", cg.uint16, 45914),
        ("y_attribute", 0x0300, 0x0004, "UINT16", cg.uint16, 19615),
    ):
        attr = cg.new_Pvariable(config[key], zb, 3, cluster,
            cg.RawExpression("EZB_ZCL_CLUSTER_SERVER"), attr_id,
            cg.RawExpression("EZB_ZCL_ATTR_TYPE_" + ztype), 1, 0)
        await cg.register_component(attr, {})
        cg.add(attr.add_attr(cg.TemplateArguments(ctype), initial))
        # Status component sends paced standard ZCL reports itself.
        attrs.append(attr)
    cg.add(var.set_attributes(*attrs))
    for attr_id, ctype, value in ((0x0008, cg.uint8, 1), (0x4001, cg.uint8, 1),
                                 (0x400A, cg.uint16, 8)):
        cg.add(zb.add_attr(cg.TemplateArguments(ctype), 3, 0x0300,
            cg.RawExpression("EZB_ZCL_CLUSTER_SERVER"), attr_id, 0, value))

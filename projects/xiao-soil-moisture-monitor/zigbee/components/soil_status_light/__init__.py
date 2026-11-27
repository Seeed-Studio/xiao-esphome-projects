"""Read-only soil color attributes, using ESPHome's native report machinery."""
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
    # This is a logical read-only indicator, not an actuator. Generic cluster
    # descriptors plus explicit types/access avoid patching SDK dispatch helpers.
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
        cg.add(attr.add_attr(cg.TemplateArguments(ctype),
            cg.RawExpression("EZB_ZCL_ATTR_ACCESS_READ | EZB_ZCL_ATTR_ACCESS_REPORTING"), initial))
        cg.add(attr.set_report(cg.RawExpression("zigbee::ZIGBEE_REPORT_ENABLE")))
        attrs.append(attr)
    cg.add(var.set_attributes(*attrs))
    # Explicit types/access are required for the generic Level/Color descriptors.
    for cluster, attr_id, ztype, ctype, value in (
        (0x0008, 0x0001, "UINT16", cg.uint16, 0),  # RemainingTime
        (0x0300, 0x0002, "UINT16", cg.uint16, 0),
        (0x0300, 0x0008, "ENUM8", cg.uint8, 1),  # XY ColorMode
        (0x0300, 0x4001, "ENUM8", cg.uint8, 1),
        (0x0300, 0x400A, "MAP16", cg.uint16, 8),  # XY capability
        (0x0001, 0x0031, "ENUM8", cg.uint8, 3),  # AA
        (0x0001, 0x0033, "UINT8", cg.uint8, 1),
        (0x0001, 0x0034, "UINT8", cg.uint8, 15),  # 1.5 V nominal
    ):
        cg.add(zb.add_attr(cg.TemplateArguments(ctype), 1 if cluster == 1 else 3,
            cluster, cg.RawExpression("EZB_ZCL_CLUSTER_SERVER"), attr_id, 0,
            cg.RawExpression("EZB_ZCL_ATTR_TYPE_" + ztype),
            cg.RawExpression("EZB_ZCL_ATTR_ACCESS_READ"), value))

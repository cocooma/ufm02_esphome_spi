
import esphome.codegen as cg
import esphome.config_validation as cv

from esphome.components import sensor
from esphome.const import (
    CONF_ID,
    DEVICE_CLASS_TEMPERATURE,
    STATE_CLASS_MEASUREMENT,
    STATE_CLASS_TOTAL_INCREASING,
    UNIT_CELSIUS,
)

from . import UFM02Component

CONF_CS_PIN = "cs_pin"
CONF_INT_PIN = "int_pin"
CONF_CLK_PIN = "clk_pin"
CONF_MISO_PIN = "miso_pin"
CONF_MOSI_PIN = "mosi_pin"

CONF_FLOW = "flow"
CONF_TOTAL_VOLUME = "total_volume"
CONF_TEMPERATURE = "temperature"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(UFM02Component),

        cv.Required(CONF_CS_PIN): cv.int_range(min=0, max=48),
        cv.Required(CONF_INT_PIN): cv.int_range(min=0, max=48),
        cv.Required(CONF_CLK_PIN): cv.int_range(min=0, max=48),
        cv.Required(CONF_MISO_PIN): cv.int_range(min=0, max=48),
        cv.Required(CONF_MOSI_PIN): cv.int_range(min=0, max=48),

        cv.Required(CONF_FLOW): sensor.sensor_schema(
            unit_of_measurement="L/h",
            accuracy_decimals=1,
            icon="mdi:water-pump",
            state_class=STATE_CLASS_MEASUREMENT,
        ),

        cv.Required(CONF_TOTAL_VOLUME): sensor.sensor_schema(
            unit_of_measurement="m³",
            accuracy_decimals=6,
            icon="mdi:water",
            state_class=STATE_CLASS_TOTAL_INCREASING,
        ),

        cv.Required(CONF_TEMPERATURE): sensor.sensor_schema(
            unit_of_measurement=UNIT_CELSIUS,
            accuracy_decimals=1,
            icon="mdi:thermometer",
            device_class=DEVICE_CLASS_TEMPERATURE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
    }
).extend(cv.COMPONENT_SCHEMA).extend(
    cv.polling_component_schema("100ms")
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    cg.add(var.set_cs_pin(config[CONF_CS_PIN]))
    cg.add(var.set_int_pin(config[CONF_INT_PIN]))
    cg.add(var.set_clk_pin(config[CONF_CLK_PIN]))
    cg.add(var.set_miso_pin(config[CONF_MISO_PIN]))
    cg.add(var.set_mosi_pin(config[CONF_MOSI_PIN]))

    flow = await sensor.new_sensor(config[CONF_FLOW])
    cg.add(var.set_flow_sensor(flow))

    volume = await sensor.new_sensor(config[CONF_TOTAL_VOLUME])
    cg.add(var.set_total_volume_sensor(volume))

    temperature = await sensor.new_sensor(config[CONF_TEMPERATURE])
    cg.add(var.set_temperature_sensor(temperature))

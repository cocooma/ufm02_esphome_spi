import esphome.codegen as cg
import esphome.config_validation as cv

from esphome import pins
from esphome.components import sensor, spi
from esphome.const import (
    CONF_ID,
    CONF_TEMPERATURE,
    DEVICE_CLASS_TEMPERATURE,
    STATE_CLASS_MEASUREMENT,
    STATE_CLASS_TOTAL_INCREASING,
    UNIT_CELSIUS,
)

DEPENDENCIES = ["spi"]

CONF_INT_PIN = "int_pin"
CONF_FLOW = "flow"
CONF_FILTERED_FLOW = "filtered_flow"
CONF_TOTAL_VOLUME = "total_volume"
CONF_ERROR_FLAGS = "error_flags"

ufm02_ns = cg.esphome_ns.namespace("ufm02")

UFM02Component = ufm02_ns.class_(
    "UFM02Component",
    cg.PollingComponent,
    spi.SPIDevice,
)

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(UFM02Component),
            cv.Required(CONF_INT_PIN): pins.gpio_input_pin_schema,

            cv.Optional(CONF_FLOW): sensor.sensor_schema(
                unit_of_measurement="L/min",
                icon="mdi:water-pump",
                accuracy_decimals=3,
                state_class=STATE_CLASS_MEASUREMENT,
            ),

            cv.Optional(CONF_FILTERED_FLOW): sensor.sensor_schema(
                unit_of_measurement="L/min",
                icon="mdi:water",
                accuracy_decimals=3,
                state_class=STATE_CLASS_MEASUREMENT,
            ),

            cv.Optional(CONF_TOTAL_VOLUME): sensor.sensor_schema(
                unit_of_measurement="L",
                icon="mdi:counter",
                accuracy_decimals=3,
                state_class=STATE_CLASS_TOTAL_INCREASING,
            ),

            cv.Optional(CONF_TEMPERATURE): sensor.sensor_schema(
                unit_of_measurement=UNIT_CELSIUS,
                device_class=DEVICE_CLASS_TEMPERATURE,
                icon="mdi:thermometer",
                accuracy_decimals=2,
                state_class=STATE_CLASS_MEASUREMENT,
            ),

            cv.Optional(CONF_ERROR_FLAGS): sensor.sensor_schema(
                icon="mdi:alert-circle",
                accuracy_decimals=0,
            ),
        }
    )
    .extend(cv.polling_component_schema("1s"))
    .extend(
        spi.spi_device_schema(
            cs_pin_required=True,
            default_mode="mode1",
            default_data_rate="1MHz",
        )
    )
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])

    await cg.register_component(var, config)
    await spi.register_spi_device(var, config)

    int_pin = await cg.gpio_pin_expression(config[CONF_INT_PIN])
    cg.add(var.set_int_pin(int_pin))

    sensor_configs = (
        (CONF_FLOW, "set_flow_sensor"),
        (CONF_FILTERED_FLOW, "set_filtered_flow_sensor"),
        (CONF_TOTAL_VOLUME, "set_total_volume_sensor"),
        (CONF_TEMPERATURE, "set_temperature_sensor"),
        (CONF_ERROR_FLAGS, "set_error_flags_sensor"),
    )

    for key, setter in sensor_configs:
        if key in config:
            sens = await sensor.new_sensor(config[key])
            cg.add(getattr(var, setter)(sens))
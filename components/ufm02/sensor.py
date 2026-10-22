
import esphome.codegen as cg
import esphome.config_validation as cv

from esphome import pins
from esphome.components import sensor, spi
from esphome.const import CONF_ID, CONF_UPDATE_INTERVAL, UNIT_EMPTY

DEPENDENCIES = ["spi"]

CONF_INT_PIN = "int_pin"
CONF_REGISTER_ADDRESS = "register_address"

ufm02_ns = cg.esphome_ns.namespace("ufm02")

UFM02Sensor = ufm02_ns.class_(
    "UFM02Sensor",
    cg.PollingComponent,
    spi.SPIDevice,
    sensor.Sensor,
)

CONFIG_SCHEMA = (
    sensor.sensor_schema(
        UFM02Sensor,
        unit_of_measurement=UNIT_EMPTY,
        accuracy_decimals=0,
    )
    .extend(
        spi.spi_device_schema(cs_pin_required=True)
    )
    .extend(
        {
            cv.Required(CONF_INT_PIN):
                pins.internal_gpio_input_pin_schema,
            cv.Required(CONF_REGISTER_ADDRESS):
                cv.hex_uint8_t,
            cv.Optional(
                CONF_UPDATE_INTERVAL, default="1s"
            ): cv.update_interval,
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])

    await cg.register_component(var, config)
    await spi.register_spi_device(var, config)
    await sensor.register_sensor(var, config)

    int_pin = await cg.gpio_pin_expression(
        config[CONF_INT_PIN]
    )
    cg.add(var.set_int_pin(int_pin))

    cg.add(
        var.set_register_address(
            config[CONF_REGISTER_ADDRESS]
        )
    )

import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import (
    CONF_ID,
    STATE_CLASS_MEASUREMENT,
    ENTITY_CATEGORY_DIAGNOSTIC,
)
from . import espaxa_ns, EspAxaComponent

CONF_ESPAXA_ID = "espaxa_id"

CONFIG_SCHEMA = cv.All(
    sensor.sensor_schema(
        accuracy_decimals=0,
        state_class=STATE_CLASS_MEASUREMENT,
        entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
    ).extend(
        {
            cv.GenerateID(CONF_ESPAXA_ID): cv.use_id(EspAxaComponent),
        }
    )
)


async def to_code(config):
    parent = await cg.get_variable(config[CONF_ESPAXA_ID])
    sens = await sensor.new_sensor(config)
    cg.add(parent.set_status_sensor(sens))
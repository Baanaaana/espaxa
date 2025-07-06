import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import text_sensor
from esphome.const import CONF_ID, ENTITY_CATEGORY_DIAGNOSTIC
from . import espaxa_ns, EspAxaComponent

CONF_ESPAXA_ID = "espaxa_id"

CONFIG_SCHEMA = cv.All(
    text_sensor.text_sensor_schema(
        entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
    ).extend(
        {
            cv.GenerateID(CONF_ESPAXA_ID): cv.use_id(EspAxaComponent),
        }
    )
)


async def to_code(config):
    parent = await cg.get_variable(config[CONF_ESPAXA_ID])
    sens = await text_sensor.new_text_sensor(config)
    cg.add(parent.set_status_text_sensor(sens))
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import text_sensor
from esphome.const import CONF_ID, ENTITY_CATEGORY_DIAGNOSTIC
from . import espaxa_ns, EspAxaComponent

CONF_ESPAXA_ID = "espaxa_id"
CONF_TYPE = "type"

TYPES = {
    "status": "set_status_text_sensor",
    "device_info": "set_device_info_text_sensor", 
    "version": "set_version_text_sensor"
}

CONFIG_SCHEMA = cv.All(
    text_sensor.text_sensor_schema(
        entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
    ).extend(
        {
            cv.GenerateID(CONF_ESPAXA_ID): cv.use_id(EspAxaComponent),
            cv.Required(CONF_TYPE): cv.one_of(*TYPES, lower=True),
        }
    )
)


async def to_code(config):
    parent = await cg.get_variable(config[CONF_ESPAXA_ID])
    sens = await text_sensor.new_text_sensor(config)
    method_name = TYPES[config[CONF_TYPE]]
    cg.add(getattr(parent, method_name)(sens))
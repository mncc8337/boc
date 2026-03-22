#include "hp_BH1750.h"
#include <BH1750_US.h>

BH1750_US::BH1750_US(hp_BH1750 &hw, int32_t id)
    : sensor_hw(hw), sensor_id(id) {}

bool BH1750_US::getEvent(sensors_event_t* event) {
    int raw = last_raw;

    if(sensor_hw.hasValue()) {
        raw = sensor_hw.getRaw();
        last_raw = raw;

        sensor_hw.adjustSettings(50);

        sensor_hw.start();
    }

    printf("quality: %d, mtreg %d\n", sensor_hw.getQuality(), sensor_hw.getMtreg());

    memset(event, 0, sizeof(sensors_event_t));
    event->version = 1;
    event->sensor_id = sensor_id;
    event->type = SENSOR_TYPE_LIGHT;
    event->timestamp = millis();
    event->light = sensor_hw.calcLux(raw);
    
    return true; 
}

void BH1750_US::getSensor(sensor_t* sensor) {
    memset(sensor, 0, sizeof(sensor_t));
    strncpy(sensor->name, "BH1750", sizeof(sensor->name) - 1);
    sensor->version = 1;
    sensor->sensor_id = sensor_id;
    sensor->type = SENSOR_TYPE_LIGHT;
    sensor->min_value = 0.0f;
    sensor->max_value = 121556.86f;
    sensor->resolution = 0.14f;
}

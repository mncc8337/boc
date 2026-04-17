#include <QMC5883P_US.h>
#include <BMI160_QMC5883P.h>

static float getRange(QMC5883P_Range range) {
    switch(range) {
        case QMC_RANGE_30G:
            return 3000;
        case QMC_RANGE_12G:
            return 1200;
        case QMC_RANGE_8G:
            return 800;
        case QMC_RANGE_2G:
            return 200;
    }
    return 0.0f;
}

QMC5883P_US_Magnetometer::QMC5883P_US_Magnetometer(BMI160_QMC5883P &sensor_hw, int32_t _id) : sensor_id(_id), sensor_hw(sensor_hw) {}


bool QMC5883P_US_Magnetometer::getEvent(sensors_event_t* event) {
    memset(event, 0, sizeof(sensors_event_t));
    
    int16_t mx, my, mz;
    sensor_hw.getMagneticField(&mx, &my, &mz);

    float range = getRange(sensor_hw.getRange());
    float res = range / 32768.0f;

    event->version = 1;
    event->sensor_id = sensor_id;
    event->type = SENSOR_TYPE_MAGNETIC_FIELD;
    event->timestamp = millis();
    event->magnetic.x = mx * res;
    event->magnetic.y = my * res;
    event->magnetic.z = mz * res;
    
    return true; 
}

void QMC5883P_US_Magnetometer::getSensor(sensor_t* sensor) {
    memset(sensor, 0, sizeof(sensor_t));
    strncpy(sensor->name, "QMC5883P", sizeof(sensor->name) - 1);

    float range = getRange(sensor_hw.getRange());
    
    sensor->version = 1;
    sensor->sensor_id = sensor_id;
    sensor->type = SENSOR_TYPE_MAGNETIC_FIELD;
    sensor->min_value = -range;
    sensor->max_value = range;
    sensor->resolution = range / 32768.0f;
}

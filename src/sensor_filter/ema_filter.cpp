#include <sensor_filter.h>

EMAFilter::EMAFilter(Adafruit_Sensor* sensor, float alpha_val) 
    : SensorFilter(sensor), alpha(alpha_val), is_initialized(false) {
    for(int i = 0; i < 3; i++) state[i] = 0.0f;
}

void EMAFilter::apply_ema(float &target, int index) {
    if(!is_initialized) {
        state[index] = target;
    } else {
        state[index] = state[index] + alpha * (target - state[index]);
    }
    target = state[index];
}

bool EMAFilter::getEvent(sensors_event_t* event) {
    if(!base_sensor->getEvent(event)) {
        return false;
    }

    switch(event->type) {
        case SENSOR_TYPE_LIGHT:
            apply_ema(event->light, 0);
            break;
        case SENSOR_TYPE_AMBIENT_TEMPERATURE:
            apply_ema(event->temperature, 0);
            break;
        case SENSOR_TYPE_RELATIVE_HUMIDITY:
            apply_ema(event->relative_humidity, 0);
            break;
        case SENSOR_TYPE_PRESSURE:
            apply_ema(event->pressure, 0);
            break;
        case SENSOR_TYPE_ACCELEROMETER:
            apply_ema(event->acceleration.x, 0);
            apply_ema(event->acceleration.y, 1);
            apply_ema(event->acceleration.z, 2);
            break;
        case SENSOR_TYPE_GYROSCOPE:
            apply_ema(event->gyro.x, 0);
            apply_ema(event->gyro.y, 1);
            apply_ema(event->gyro.z, 2);
            break;
    }
    
    is_initialized = true;
    return true;
}

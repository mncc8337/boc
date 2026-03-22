#include <sensor_filter.h>

SMAFilter::SMAFilter(Adafruit_Sensor* sensor, uint8_t size) 
    : SensorFilter(sensor), window_size(size), head(0), is_full(false) {
    
    buffer = new float*[3];
    for(int i = 0; i < 3; i++) {
        buffer[i] = new float[window_size]{0.0f};
        sum[i] = 0.0f;
    }
}

SMAFilter::~SMAFilter() {
    for(int i = 0; i < 3; i++) delete[] buffer[i];
    delete[] buffer;
}

void SMAFilter::apply_sma(float &target, int axis) {
    sum[axis] -= buffer[axis][head];
    buffer[axis][head] = target;
    sum[axis] += target;
    target = sum[axis] / (is_full ? window_size : (head + 1));
}

bool SMAFilter::getEvent(sensors_event_t* event) {
    if(!base_sensor->getEvent(event)) return false;

    switch(event->type) {
        case SENSOR_TYPE_AMBIENT_TEMPERATURE:
            apply_sma(event->temperature, 0);
            break;
        case SENSOR_TYPE_RELATIVE_HUMIDITY:
            apply_sma(event->relative_humidity, 0);
            break;
        case SENSOR_TYPE_LIGHT:
            apply_sma(event->light, 0);
            break;
        case SENSOR_TYPE_ACCELEROMETER:
            apply_sma(event->acceleration.x, 0);
            apply_sma(event->acceleration.y, 1);
            apply_sma(event->acceleration.z, 2);
            break;
        case SENSOR_TYPE_GYROSCOPE:
            apply_sma(event->gyro.x, 0);
            apply_sma(event->gyro.y, 1);
            apply_sma(event->gyro.z, 2);
            break;
    }
    
    head++;
    if(head >= window_size) {
        head = 0;
        is_full = true;
    }
    
    return true;
}

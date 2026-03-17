#include <BMI160_US.h>

bool BMI160_US_Accelerometer::getEvent(sensors_event_t* event) {
    memset(event, 0, sizeof(sensors_event_t));
    
    int ax, ay, az;
    BMI160.readAccelerometer(ax, ay, az);

    float range_g = BMI160.getAccelerometerRange(); 
    float res = (range_g * 9.80665f) / 32768.0f;

    event->version = 1;
    event->sensor_id = sensor_id;
    event->type = SENSOR_TYPE_ACCELEROMETER;
    event->timestamp = millis();
    event->acceleration.x = ax * res;
    event->acceleration.y = ay * res;
    event->acceleration.z = az * res;
    
    return true; 
}

void BMI160_US_Accelerometer::getSensor(sensor_t* sensor) {
    memset(sensor, 0, sizeof(sensor_t));
    strncpy(sensor->name, "BMI160", sizeof(sensor->name) - 1);
    
    float range_g = BMI160.getAccelerometerRange();
    float max_val = range_g * 9.80665f;

    sensor->version = 1;
    sensor->sensor_id = sensor_id;
    sensor->type = SENSOR_TYPE_ACCELEROMETER;
    sensor->min_value = -max_val;
    sensor->max_value = max_val;
    sensor->resolution = max_val / 32768.0f;
}

bool BMI160_US_Gyroscope::getEvent(sensors_event_t* event) {
    memset(event, 0, sizeof(sensors_event_t));
    
    int gx, gy, gz;
    BMI160.readGyro(gx, gy, gz);

    float range_dps = BMI160.getGyroRange(); 
    float res = (range_dps * (PI / 180.0f)) / 32768.0f;

    event->version = 1;
    event->sensor_id = sensor_id;
    event->type = SENSOR_TYPE_GYROSCOPE;
    event->timestamp = millis();
    event->gyro.x = gx * res;
    event->gyro.y = gy * res;
    event->gyro.z = gz * res;
    
    return true; 
}

void BMI160_US_Gyroscope::getSensor(sensor_t* sensor) {
    memset(sensor, 0, sizeof(sensor_t));
    strncpy(sensor->name, "BMI160", sizeof(sensor->name) - 1);

    float range_dps = BMI160.getGyroRange();
    float max_val = range_dps * (PI / 180.0f);

    sensor->version = 1;
    sensor->sensor_id = sensor_id;
    sensor->type = SENSOR_TYPE_GYROSCOPE;
    sensor->min_value = -max_val;
    sensor->max_value = max_val;
    sensor->resolution = max_val / 32768.0f;
}

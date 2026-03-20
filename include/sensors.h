#ifndef SENSORS_H
#define SENSORS_H

#include <stdint.h>

class Adafruit_Sensor;

enum SensorEnum {
    // SENS_BMP280_TEMPERATURE,
    SENS_AHTX0_TEMPERATURE,
    SENS_AHTX0_HUMIDITY,
    SENS_BMP280_PRESSURE,
    SENS_BH1750,
    SENS_BMI160_ACCELERATION,
    SENS_BMI160_GYROSCOPE,
    SENS_COUNT // put this at the end of the list
};

typedef struct {
    float temperature;
    float humidity;
    float pressure;
    float light;
    float accel[3];
    float gyro[3];
    uint16_t active_mask;
} sensors_data_t;

extern Adafruit_Sensor *sensors[SENS_COUNT];

extern bool init_sensors();

extern void sleep_sensors();
extern void wake_sensors();

extern void set_low_power_sensor_mode();
extern void unset_low_power_sensor_mode();

extern void get_sensors_data(sensors_data_t &data);

#endif

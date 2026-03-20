#ifndef SENSORS_H
#define SENSORS_H

#include <vector>
#include <stdint.h>

class Adafruit_Sensor;

enum SensorEnum {
    // SENS_BMP280_TEMPERATURE,
    SENS_BMP280_PRESSURE,
    SENS_AHTX0_TEMPERATURE,
    SENS_AHTX0_HUMIDITY,
    SENS_BH1750,
    SENS_BMI160_ACCELERATION,
    SENS_BMI160_GYROSCOPE,
    SENS_COUNT // put this at the end of the list
};

extern Adafruit_Sensor *sensors[SENS_COUNT];

extern bool init_sensors();

extern void sleep_sensors();
extern void wake_sensors();

extern void set_low_power_sensor_mode();
extern void unset_low_power_sensor_mode();

extern void pack_telemetry_payload(std::vector<uint8_t>& payload);

#endif

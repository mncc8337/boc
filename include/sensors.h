#ifndef SENSORS_H
#define SENSORS_H

#define SENSOR_ALIVE(id) ((sensor_mask >> id) & 1)
#define SENSOR_ACTIVE(id) ((lognsend_mask >> id) & 1)

#include <stdint.h>
#include <Adafruit_Sensor.h>

class Adafruit_Sensor;

enum SensorEnum {
    SENS_LIGHT,
    SENS_TEMPERATURE,
    SENS_HUMIDITY,
    SENS_PRESSURE,
    SENS_ACCELERATION,
    SENS_GYROSCOPE,
    SENS_COUNT // put this at the end of the list
};

typedef struct {
    float temperature;
    float humidity;
    float pressure;
    float light;
    float accel[3];
    float gyro[3];
} sensors_data_t;

typedef uint16_t sensor_mask_t;

extern Adafruit_Sensor *sensors[SENS_COUNT];
extern sensor_mask_t sensor_mask;
extern sensor_mask_t lognsend_mask;

extern const char* get_sensor_type_string(const int &type);
extern const char* get_sensor_unit_string(const int type);
extern uint16_t sensors_init();

extern void sleep_sensors();
extern void wake_sensors();

extern void set_low_power_sensor_mode();
extern void unset_low_power_sensor_mode();

extern void subscribe_live_data(int target_sensor, unsigned long sampling_interval);
extern void unsubscribe_live_data();
extern void set_live_data_sampling_interval(unsigned long sampling_interval);
extern bool live_data_ready(sensors_event_t &out_event);
extern bool all_data_poll_ready(sensors_data_t &out_data);

extern TaskHandle_t sensors_task_handle;
extern void sensors_task(void *parameters);

#endif

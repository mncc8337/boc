#ifndef BMI160_US_H
#define BMI160_US_H

#include <Adafruit_Sensor.h>
#include <BMI160Gen.h>

class BMI160_US_Accelerometer : public Adafruit_Sensor {
private:
    int32_t sensor_id;
public:
    BMI160_US_Accelerometer(int32_t _id) : sensor_id(_id) {}
    bool getEvent(sensors_event_t* event) override;
    void getSensor(sensor_t* sensor) override;
};

class BMI160_US_Gyroscope : public Adafruit_Sensor {
private:
    int32_t sensor_id;
public:
    BMI160_US_Gyroscope(int32_t _id) : sensor_id(_id) {}
    bool getEvent(sensors_event_t* event) override;
    void getSensor(sensor_t* sensor) override;
};

#endif

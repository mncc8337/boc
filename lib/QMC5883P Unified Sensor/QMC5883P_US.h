#ifndef QMC5883P_US_H
#define QMC5883P_US_H

#include <Adafruit_Sensor.h>
#include <BMI160_QMC5883P.h>

class QMC5883P_US_Magnetometer : public Adafruit_Sensor {
private:
    int32_t sensor_id;
    BMI160_QMC5883P &sensor_hw;
public:
    QMC5883P_US_Magnetometer(BMI160_QMC5883P &sensor_hw, int32_t _id);
    bool getEvent(sensors_event_t* event) override;
    void getSensor(sensor_t* sensor) override;
};

#endif

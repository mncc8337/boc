#ifndef BH1750_US_H
#define BH1750_US_H

#include <Adafruit_Sensor.h>
#include <BH1750.h>

class BH1750_US : public Adafruit_Sensor {
private:
    BH1750 &sensor_hw;
    int32_t sensor_id;
public:
    BH1750_US(BH1750 &hw, int32_t id);

    bool getEvent(sensors_event_t* event) override;

    void getSensor(sensor_t* sensor) override;
};

#endif

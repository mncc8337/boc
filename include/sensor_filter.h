#ifndef SENSOR_FILTER_H
#define SENSOR_FILTER_H

#include <Adafruit_Sensor.h>

class SensorFilter : public Adafruit_Sensor {
protected:
    Adafruit_Sensor* base_sensor;

public:
    SensorFilter(Adafruit_Sensor* sensor) : base_sensor(sensor) {}

    virtual bool getEvent(sensors_event_t* event) override {
        return base_sensor->getEvent(event);
    }
};

class EMAFilter : public SensorFilter {
private:
    float alpha;
    bool is_initialized;
    float state[3];

    void apply_ema(float &target, int index);

public:
    EMAFilter(Adafruit_Sensor* sensor, float alpha_val = 0.2f);
    virtual bool getEvent(sensors_event_t* event) override;
};

class SMAFilter : public SensorFilter {
private:
    uint8_t window_size;
    uint8_t head;
    bool is_full;
    float** buffer; 
    float sum[3];

    void apply_sma(float &target, int axis);

public:
    SMAFilter(Adafruit_Sensor* sensor, uint8_t size = 10);
    ~SMAFilter();
    virtual bool getEvent(sensors_event_t* event) override;
};

#endif

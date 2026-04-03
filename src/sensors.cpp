#include <sensors.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_AHTX0.h>
#include <hp_BH1750.h>
#include <BH1750_US.h>
#include <BMI160_US.h>

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

#include <esp_log.h>

Adafruit_BMP280 bmp280;
Adafruit_AHTX0 ahtx0;
hp_BH1750 bh1750_hw;
BH1750_US bh1750(bh1750_hw, 1750);
BMI160_US_Accelerometer bmi160_accel(160);
BMI160_US_Gyroscope bmi160_gyro(160);

Adafruit_Sensor *sensors[SENS_COUNT];
sensor_mask_t sensor_mask;
sensor_mask_t lognsend_mask = 0;

Adafruit_BMP280::sensor_sampling bmp280_sampling = Adafruit_BMP280::SAMPLING_X16;
Adafruit_BMP280::sensor_filter bmp280_filter = Adafruit_BMP280::FILTER_X4;
Adafruit_BMP280::standby_duration bmp280_standby_duration = Adafruit_BMP280::STANDBY_MS_1;

int bmi160_accel_range = BMI160_ACCEL_RANGE_2G;
int bmi160_accel_odr = BMI160_ACCEL_RATE_100HZ;

int bmi160_gyro_range = BMI160_GYRO_RANGE_2000;
int bmi160_gyro_odr = BMI160_GYRO_RATE_50HZ;

QueueHandle_t live_data_event_queue;
volatile int live_data_sensor_id = -1;

QueueHandle_t all_lognsend_sensor_data_queue;

const char* get_sensor_type_string(const int &type) {
    switch(type) {
        case SENSOR_TYPE_ACCELEROMETER: return "Accelerometer";
        case SENSOR_TYPE_MAGNETIC_FIELD: return "Magnetic Field";
        case SENSOR_TYPE_ORIENTATION: return "Orientation";
        case SENSOR_TYPE_GYROSCOPE: return "Gyroscope";
        case SENSOR_TYPE_LIGHT: return "Light";
        case SENSOR_TYPE_PRESSURE: return "Pressure";
        case SENSOR_TYPE_PROXIMITY: return "Proximity";
        case SENSOR_TYPE_GRAVITY: return "Gravity";
        case SENSOR_TYPE_LINEAR_ACCELERATION: return "Linear Acceleration";
        case SENSOR_TYPE_ROTATION_VECTOR: return "Rotation Vector";
        case SENSOR_TYPE_RELATIVE_HUMIDITY: return "Relative Humidity";
        case SENSOR_TYPE_AMBIENT_TEMPERATURE: return "Ambient Temperature";
        case SENSOR_TYPE_OBJECT_TEMPERATURE: return "Object Temperature";
        case SENSOR_TYPE_VOLTAGE: return "Voltage";
        case SENSOR_TYPE_CURRENT: return "Current";
        case SENSOR_TYPE_COLOR: return "Color";
        case SENSOR_TYPE_TVOC: return "TVOC";
        case SENSOR_TYPE_VOC_INDEX: return "VOC Index";
        case SENSOR_TYPE_NOX_INDEX: return "NOX Index";
        case SENSOR_TYPE_CO2: return "CO2";
        case SENSOR_TYPE_ECO2: return "eCO2";
        case SENSOR_TYPE_PM10_STD: return "PM10 STD";
        case SENSOR_TYPE_PM25_STD: return "PM25 STD";
        case SENSOR_TYPE_PM100_STD: return "PM100 STD";
        case SENSOR_TYPE_PM10_ENV: return "PM10 ENV";
        case SENSOR_TYPE_PM25_ENV: return "PM25 ENV";
        case SENSOR_TYPE_PM100_ENV: return "PM100 ENV";
        case SENSOR_TYPE_GAS_RESISTANCE: return "Gas Resistance";
        case SENSOR_TYPE_UNITLESS_PERCENT: return "Unitless Percent";
        case SENSOR_TYPE_ALTITUDE: return "Altitude";
        default: return "Unknown";
    }
}

const char* get_sensor_unit_string(const int type) {
    switch(type) {
        case SENSOR_TYPE_ACCELEROMETER: 
        case SENSOR_TYPE_GRAVITY: 
        case SENSOR_TYPE_LINEAR_ACCELERATION: 
            return "m/s2";
            
        case SENSOR_TYPE_MAGNETIC_FIELD: return "uT";
        case SENSOR_TYPE_ORIENTATION: return "deg";
        case SENSOR_TYPE_GYROSCOPE: return "rad/s";
        case SENSOR_TYPE_LIGHT: return "Lux";
        case SENSOR_TYPE_PRESSURE: return "hPa";
        case SENSOR_TYPE_PROXIMITY: return "cm";
        case SENSOR_TYPE_RELATIVE_HUMIDITY: return "%";
        case SENSOR_TYPE_UNITLESS_PERCENT: return "%";
        
        case SENSOR_TYPE_AMBIENT_TEMPERATURE: 
        case SENSOR_TYPE_OBJECT_TEMPERATURE: 
            return "\xB0""C";
            
        case SENSOR_TYPE_VOLTAGE: return "V";
        case SENSOR_TYPE_CURRENT: return "mA";
        case SENSOR_TYPE_TVOC: return "ppb";

        case SENSOR_TYPE_CO2: 
        case SENSOR_TYPE_ECO2:
            return "ppm";
        
        case SENSOR_TYPE_PM10_STD:
        case SENSOR_TYPE_PM25_STD:
        case SENSOR_TYPE_PM100_STD:
        case SENSOR_TYPE_PM10_ENV:
        case SENSOR_TYPE_PM25_ENV:
        case SENSOR_TYPE_PM100_ENV: 
            return "ug/m3";
            
        case SENSOR_TYPE_GAS_RESISTANCE: return "Ohm";
        case SENSOR_TYPE_ALTITUDE: return "m";
        case SENSOR_TYPE_COLOR: return "rgb";

        case SENSOR_TYPE_ROTATION_VECTOR: 
        case SENSOR_TYPE_VOC_INDEX: 
        case SENSOR_TYPE_NOX_INDEX: 
        default:
            return "";
    }
}

static void bmi160_on() {
    BMI160.setRegister(0x7E, 0x11); delay(50);
    BMI160.setRegister(0x7E, 0x15); delay(50);
}

static void bmi160_off() {
    BMI160.setRegister(0x7E, 0x10); delay(50);
    BMI160.setRegister(0x7E, 0x14); delay(50);
}

TaskHandle_t sensors_task_handle = nullptr;

uint16_t sensors_init() {
    sensor_mask = 0;
    for(int i = 0; i < SENS_COUNT; i++) {
        sensors[i] = nullptr;
    }

    if(bh1750_hw.begin(0x23)) {
        sensor_mask |= 1 << SENS_LIGHT;
        sensors[SENS_LIGHT] = &bh1750;
        bh1750_hw.calibrateTiming();
        bh1750_hw.start(BH1750_QUALITY_HIGH2, 69);
        ESP_LOGI("SENSORS", "BH1750 initialized");
    } else {
        ESP_LOGE("SENSORS", "BH1750 failed to initialize");
    }

    if(ahtx0.begin()) {
        sensor_mask |= 1 << SENS_TEMPERATURE;
        sensor_mask |= 1 << SENS_HUMIDITY;
        sensors[SENS_TEMPERATURE] = ahtx0.getTemperatureSensor();
        sensors[SENS_HUMIDITY] = ahtx0.getHumiditySensor();
        ESP_LOGI("SENSORS", "AHT20 initialized");
    } else {
        ESP_LOGE("SENSORS", "AHT20 failed to initialize");
    }

    if(bmp280.begin(0x77)) {
        sensor_mask |= 1 << SENS_PRESSURE;
        sensors[SENS_PRESSURE] = bmp280.getPressureSensor();

        bmp280.setSampling(
            Adafruit_BMP280::MODE_NORMAL,
            Adafruit_BMP280::SAMPLING_X2,
            bmp280_sampling,
            bmp280_filter,
            bmp280_standby_duration
        );
        ESP_LOGI("SENSORS", "BMP280 initialized");
    } else {
        ESP_LOGE("SENSORS", "BMP280 failed to initialize");
    }

    if(BMI160.begin(BMI160GenClass::I2C_MODE, 0x69)) {
        sensor_mask |= 1 << SENS_ACCELERATION;
        sensor_mask |= 1 << SENS_GYROSCOPE;
        sensors[SENS_ACCELERATION] = &bmi160_accel;
        sensors[SENS_GYROSCOPE] = &bmi160_gyro;
        BMI160.setFullScaleAccelRange(bmi160_accel_range); 
        BMI160.setAccelRate(bmi160_accel_odr);

        BMI160.setFullScaleGyroRange(bmi160_gyro_range);
        BMI160.setGyroRange(bmi160_gyro_range);
        BMI160.setGyroRate(bmi160_gyro_odr);
        ESP_LOGI("SENSORS", "BMI160 initialized");
    } else {
        ESP_LOGE("SENSORS", "BMI160 failed to initialize");
    }

    // setup rtos task stuff
    live_data_event_queue = xQueueCreate(1, sizeof(sensors_event_t));
    all_lognsend_sensor_data_queue =  xQueueCreate(1, sizeof(sensors_data_t));

    lognsend_mask = sensor_mask;
    return sensor_mask;
}

void sleep_sensors() {
    if(SENSOR_ALIVE(SENS_PRESSURE))
        bmp280.setSampling(Adafruit_BMP280::MODE_SLEEP);

    // ahtx0 and bh1750 dont need to sleep

    if(SENSOR_ALIVE(SENS_ACCELERATION))
        bmi160_off();

    ESP_LOGI("SENSORS", "Set all sensors to sleep mode");
}

void wake_sensors() {
    if(SENSOR_ALIVE(SENS_PRESSURE))
        bmp280.setSampling(Adafruit_BMP280::MODE_NORMAL);

    if(SENSOR_ALIVE(SENS_ACCELERATION))
        bmi160_on();

    ESP_LOGI("SENSORS", "All sensors waken");
}

void set_low_power_sensor_mode() {
    if(SENSOR_ALIVE(SENS_ACCELERATION) && !SENSOR_ACTIVE(SENS_ACCELERATION))
        bmi160_off();

    ESP_LOGI("SENSORS", "Set all sensors to low power mode");
}

void unset_low_power_sensor_mode() {
    if(SENSOR_ALIVE(SENS_ACCELERATION) && !SENSOR_ACTIVE(SENS_ACCELERATION))
        bmi160_on();

    ESP_LOGI("SENSORS", "Unset all sensors to low power mode");
}

static unsigned long live_data_sampling_interval = 0;
void subscribe_live_data(int target_sensor, unsigned long sampling_interval) {
    xQueueReset(live_data_event_queue);
    live_data_sensor_id = target_sensor;
    live_data_sampling_interval = sampling_interval;

    if(sensors_task_handle != NULL) xTaskNotifyGive(sensors_task_handle); 
}

void unsubscribe_live_data() {
    live_data_sensor_id = -1;
}

void set_live_data_sampling_interval(unsigned long sampling_interval) {
    live_data_sampling_interval = sampling_interval;
    if(sensors_task_handle != NULL) xTaskNotifyGive(sensors_task_handle); 
}

bool is_live_data_ready(sensors_event_t &out_event) {
    if(xQueueReceive(live_data_event_queue, &out_event, 2) == pdPASS) {
        return true;
    }
    return false;
}

static unsigned long session_data_sampling_interval = 100;
void set_session_data_sampling_interval(unsigned long sampling_interval) {
    session_data_sampling_interval = sampling_interval;
    if(sensors_task_handle != NULL) xTaskNotifyGive(sensors_task_handle); 
}

bool is_session_data_ready(sensors_data_t &out_data) {
    if(xQueueReceive(all_lognsend_sensor_data_queue, &out_data, 2) == pdPASS) {
        return true;
    }
    return false;
}

extern bool is_session_running;
void sensors_task(void *parameters) {
    uint8_t ready_mask = 0;
    sensors_data_t all_data = {0};
    sensors_event_t t_event;

    long time_till_live = 0;
    long time_till_session = 0;

    unsigned long last_iterate_ts = millis();

    while(true) {
        unsigned long now_ts = millis();
        unsigned long last_iterate_time = now_ts - last_iterate_ts;
        last_iterate_ts = now_ts;

        bool live_data_needed = (
            live_data_sensor_id >= 0 && SENSOR_ALIVE(live_data_sensor_id)
        );

        if(live_data_needed)
            time_till_live -= last_iterate_time;
        else
            time_till_live = 1;

        if(is_session_running)
            time_till_session -= last_iterate_time;
        else
            time_till_session = 1;

        unsigned long execution_start = now_ts;

        if(is_session_running) {
            if(time_till_session <= 0 || ready_mask != 0) {
                for(unsigned i = 0; i < SENS_COUNT; i++) {
                    if(
                        !SENSOR_ALIVE(i)
                        || !SENSOR_ACTIVE(i)
                        || ((ready_mask >> i) & 1)
                    ) continue;

                    bool ready = sensors[i]->getEvent(&t_event);
                    if(!ready) continue;

                    t_event.sensor_id = i;
                    ready_mask |= 1 << i;

                    if(i == live_data_sensor_id && live_data_needed) {
                        xQueueOverwrite(live_data_event_queue, &t_event);
                        time_till_live += live_data_sampling_interval;
                        if(time_till_live < 0) time_till_live = 0;
                    }

                    switch(i) {
                        case SENS_TEMPERATURE:
                            all_data.temperature = t_event.temperature;
                            break;
                        case SENS_HUMIDITY:
                            all_data.humidity = t_event.relative_humidity;
                            break;
                        case SENS_PRESSURE:
                            all_data.pressure = t_event.pressure;
                            break;
                        case SENS_LIGHT:
                            all_data.light = t_event.light;
                            break;
                        case SENS_ACCELERATION: 
                            all_data.accel[0] = t_event.acceleration.x;
                            all_data.accel[1] = t_event.acceleration.y;
                            all_data.accel[2] = t_event.acceleration.z;
                            break;
                        case SENS_GYROSCOPE:
                            all_data.gyro[0] = t_event.gyro.x;
                            all_data.gyro[1] = t_event.gyro.y;
                            all_data.gyro[2] = t_event.gyro.z;
                            break;
                    }
                }

                if(ready_mask == lognsend_mask) {
                    xQueueOverwrite(all_lognsend_sensor_data_queue, &all_data);
                    ready_mask = 0;
                    all_data = {0};
                    time_till_session += session_data_sampling_interval;
                    if(time_till_session < 0) time_till_session = 0;
                }
            }
        } else {
            ready_mask = 0;
        }

        if(live_data_needed) {
            if(time_till_live <= 0) {
                bool ready = sensors[live_data_sensor_id]->getEvent(&t_event);

                if(ready) {
                    t_event.sensor_id = live_data_sensor_id;
                    xQueueOverwrite(live_data_event_queue, &t_event);
                    time_till_live += live_data_sampling_interval;
                    if(time_till_live < 0) time_till_live = 0;
                }
            }
        }

        unsigned long execution_time = millis() - execution_start;
        long time_to_sleep = 1000;

        if(live_data_needed) {
            if(time_till_live < time_to_sleep) time_to_sleep = time_till_live;
        }

        if(is_session_running) {
            if(ready_mask != 0) {
                time_to_sleep = 10;
            } else if(time_till_session < time_to_sleep) {
                time_to_sleep = time_till_session;
            }
        }

        time_to_sleep -= execution_time;
        if(time_to_sleep < 10) time_to_sleep = 10;

        ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(time_to_sleep));
    }

    vTaskDelete(NULL);
}

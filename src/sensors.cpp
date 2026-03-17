#include <sensors.h>
#include <BH1750_US.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_AHTX0.h>
#include <BMI160_US.h>

Adafruit_BMP280 bmp280;
Adafruit_AHTX0 ahtx0;
BH1750 bh1750_hw;
BH1750_US bh1750(bh1750_hw, 100);
BMI160_US_Accelerometer bmi160_accel(103);
BMI160_US_Gyroscope bmi160_gyro(104);

Adafruit_Sensor *sensors[SENS_COUNT];

bool init_sensors() {
    if(!bh1750_hw.begin(BH1750::CONTINUOUS_HIGH_RES_MODE_2, 0x23)) {
        return false;
    }

    if(!ahtx0.begin()) {
        return false;
    }

    if(!bmp280.begin(0x77)) {
        return false;
    }

    if(!BMI160.begin(BMI160GenClass::I2C_MODE, 0x69)) {
        return false;
    } else {
        BMI160.setFullScaleAccelRange(BMI160_ACCEL_RANGE_4G); 
        BMI160.setGyroRange(1000);
    }

    sensors[SENS_BMP280_TEMPERATURE] = bmp280.getTemperatureSensor();
    sensors[SENS_BMP280_PRESSURE] = bmp280.getPressureSensor();
    sensors[SENS_AHTX0_TEMPERATURE] = ahtx0.getTemperatureSensor();
    sensors[SENS_AHTX0_HUMIDITY] = ahtx0.getHumiditySensor();
    sensors[SENS_BH1750] = &bh1750;
    sensors[SENS_BMI160_ACCELERATION] = &bmi160_accel;
    sensors[SENS_BMI160_GYROSCOPE] = &bmi160_gyro;

    return true;
}

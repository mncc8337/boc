#include <data_logger.h>

void write_log_packet(File &file, sensors_data_t &data) {
    struct __attribute__((packed)) {
        uint32_t start_code;
        unsigned long ts_unix;
        uint8_t sensor_mask;
    } header;

    header.start_code = 0x424f4220;
    header.ts_unix = time(NULL);
    header.sensor_mask = lognsend_mask;

    file.write((const uint8_t *)&header, sizeof(header));

    for(int i = 0; i < SENS_COUNT; i++) {
        if(!SENSOR_ACTIVE(i)) continue;

        switch(i) {
            case SENS_LIGHT:
                file.write((const uint8_t *)&data.light, sizeof(float));
                break;
            case SENS_TEMPERATURE:
                file.write((const uint8_t *)&data.temperature, sizeof(float));
                break;
            case SENS_HUMIDITY:
                file.write((const uint8_t *)&data.humidity, sizeof(float));
                break;
            case SENS_PRESSURE:
                file.write((const uint8_t *)&data.pressure, sizeof(float));
                break;
            case SENS_ACCELERATION:
                file.write((const uint8_t *)data.accel, sizeof(float) * 3);
                break;
            case SENS_GYROSCOPE:
                file.write((const uint8_t *)data.gyro, sizeof(float) * 3);
                break;
        }
    }
}

#include <connectivity.h>
#include <NimBLEDevice.h>
#include <sensors.h>

const char ACC_UUID[] = "987e092b-840d-4bba-8e09-07e904bd4903";
const char GYR_UUID[] = "9520136e-3e6a-475e-82dc-c73130bfe46f";
const char MAG_UUID[] = "e9cfa89f-9657-4c72-8430-72d1502b4fac";

static NimBLEServer* ble_server = nullptr;

static NimBLECharacteristic* bat_characteristic = nullptr;
static NimBLECharacteristic* temp_characteristic = nullptr;
static NimBLECharacteristic* hum_characteristic = nullptr;
static NimBLECharacteristic* press_characteristic = nullptr;
static NimBLECharacteristic* light_characteristic = nullptr;
static NimBLECharacteristic* accel_characteristic = nullptr;
static NimBLECharacteristic* gyro_characteristic = nullptr;
static NimBLECharacteristic* mag_characteristic = nullptr;

NimBLECharacteristic **sensor_characteristic_map[SENS_COUNT] {nullptr};

volatile bool device_connected = false;

class ServerCallbacks: public NimBLEServerCallbacks {
    void onConnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo) override {
        device_connected = true;
    };

    void onDisconnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo, int reason) override {
        device_connected = false;
        NimBLEDevice::startAdvertising();
    }
};

void ble_server_start() {
    NimBLEDevice::init("BOC");
    ble_server = NimBLEDevice::createServer();
    ble_server->setCallbacks(new ServerCallbacks());

    NimBLEService* battery_service = ble_server->createService("180F");
    bat_characteristic = battery_service->createCharacteristic(
        "2A19", NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY
    );

    NimBLEService* env_service = ble_server->createService("181A");
    temp_characteristic = env_service->createCharacteristic(
        "2A6E", NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY
    );
    hum_characteristic = env_service->createCharacteristic(
        "2A6F", NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY
    );
    press_characteristic = env_service->createCharacteristic(
        "2A6D", NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY
    );
    light_characteristic = env_service->createCharacteristic(
        "2AFB", NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY
    );

    NimBLEService* measurement_service = ble_server->createService("185A");
    accel_characteristic = measurement_service->createCharacteristic(
        ACC_UUID, NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY
    );
    gyro_characteristic = measurement_service->createCharacteristic(
        GYR_UUID, NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY
    );
    mag_characteristic = measurement_service->createCharacteristic(
        MAG_UUID, NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY
    );

    sensor_characteristic_map[SENS_LIGHT] = &light_characteristic;
    sensor_characteristic_map[SENS_TEMPERATURE] = &temp_characteristic;
    sensor_characteristic_map[SENS_HUMIDITY] = &hum_characteristic;
    sensor_characteristic_map[SENS_PRESSURE] = &press_characteristic;
    sensor_characteristic_map[SENS_ACCELERATION] = &accel_characteristic;
    sensor_characteristic_map[SENS_GYROSCOPE] = &gyro_characteristic;
    sensor_characteristic_map[SENS_MAGNETIC_FIELD] = &mag_characteristic;

    NimBLEAdvertising* advertising = NimBLEDevice::getAdvertising();
    advertising->reset();
    advertising->setName("BOC");
    advertising->addServiceUUID("180F");
    advertising->addServiceUUID("181A");
    advertising->addServiceUUID("185A");

    advertising->setAppearance(0x552);
    advertising->enableScanResponse(true);
    advertising->start();
}

void ble_server_update(const sensors_data_t &data, const uint8_t bat_level) {
    if(!device_connected) return; 

    for(unsigned i = 0; i < SENS_COUNT; i++) {
        if(!SENSOR_ALIVE(i) || !SENSOR_ACTIVE(i)) continue; 
        
        if(!sensor_characteristic_map[i]) continue;
        NimBLECharacteristic *characteristic = *sensor_characteristic_map[i];
        if(!characteristic) continue;

        switch(i) {
            case SENS_LIGHT: {
                uint32_t light_val = (uint32_t)(data.light * 100.0f + 0.5f);
                characteristic->setValue((uint8_t*)&light_val, 3);
                break;
            }
            case SENS_TEMPERATURE: {
                int16_t temp_val = (int16_t)(data.temperature * 100 + 0.5f);
                characteristic->setValue((uint8_t*)&temp_val, 2);
                break;
            }
            case SENS_HUMIDITY: {
                uint16_t humid_val = (uint16_t)(data.humidity * 100 + 0.5f);
                characteristic->setValue((uint8_t*)&humid_val, 2);
                break;
            }
            case SENS_PRESSURE: {
                uint32_t press_val = (uint32_t)(data.pressure * 1000 + 0.5f);
                characteristic->setValue((uint8_t*)&press_val, 4);
                break;
            }
            case SENS_ACCELERATION: {
                int16_t accel_data[3];
                for(unsigned k = 0; k < 3; k++)
                    accel_data[k] = (int16_t)(data.accel[k] * 100.0f + 0.5f);
                characteristic->setValue((uint8_t*)accel_data, 6);
                break;
            }
            case SENS_GYROSCOPE: {
                int16_t gyro_data[3];
                for(unsigned k = 0; k < 3; k++)
                    gyro_data[k] = (int16_t)(data.gyro[k] * 100.0f + 0.5f);
                characteristic->setValue((uint8_t*)gyro_data, 6);
                break;
            }
            case SENS_MAGNETIC_FIELD: {
                int16_t mag_data[3];
                for(unsigned k = 0; k < 3; k++)
                    mag_data[k] = (int16_t)(data.mag[k] * 100.0f + 0.5f);
                characteristic->setValue((uint8_t*)mag_data, 6);
                break;
            }
        }

        characteristic->notify();
        vTaskDelay(pdMS_TO_TICKS(5));
    }

    bat_characteristic->setValue(&bat_level, 1);
    bat_characteristic->notify();
}

void ble_server_stop() {
    if(ble_server != nullptr) {
        NimBLEAdvertising* advertising = NimBLEDevice::getAdvertising();
        if (advertising->isAdvertising()) {
            advertising->stop();
        }

        std::vector<uint16_t> connected_peers = ble_server->getPeerDevices();
        if (connected_peers.size() > 0) {
            for(size_t i = 0; i < connected_peers.size(); i++) {
                ble_server->disconnect(connected_peers[i]);
            }
        }

        device_connected = false;
    }

    NimBLEDevice::deinit(true);

    ble_server = nullptr;
    bat_characteristic = nullptr;
    temp_characteristic = nullptr;
    hum_characteristic = nullptr;
    press_characteristic = nullptr;
    light_characteristic = nullptr;
    accel_characteristic = nullptr;
    gyro_characteristic = nullptr;
    mag_characteristic = nullptr;
}

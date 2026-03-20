#include <connectivity.h>
#include <NimBLEDevice.h>
#include <NimBLEUtils.h>
#include <NimBLEServer.h>

static NimBLEServer* ble_server = nullptr;
static NimBLECharacteristic* temp_characteristic = nullptr;
static NimBLECharacteristic* bat_characteristic = nullptr;

void ble_server_start() {
    NimBLEDevice::init("SBOX"); 

    ble_server = NimBLEDevice::createServer();

    NimBLEService* battery_service = ble_server->createService("180F");
    
    bat_characteristic = battery_service->createCharacteristic(
        "2A19",
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY
    );
    
    uint8_t bat_level = 12;
    bat_characteristic->setValue(&bat_level, 1);
    battery_service->start();

    NimBLEService* env_service = ble_server->createService("181A");
    temp_characteristic = env_service->createCharacteristic(
        "2A6E",
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY
    );
    
    int16_t temp_val = 2500;
    temp_characteristic->setValue((uint8_t*)&temp_val, 2);
    
    env_service->start();

    NimBLEAdvertising* advertising = NimBLEDevice::getAdvertising();
    advertising->addServiceUUID("180F");
    advertising->addServiceUUID("181A");
    
    advertising->start();
}


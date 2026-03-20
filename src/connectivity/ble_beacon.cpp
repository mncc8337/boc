#include <connectivity.h>
#include <NimBLEDevice.h>
#include <NimBLEUtils.h>

void ble_beacon_start() {
    NimBLEAdvertising *advertising = NimBLEDevice::getAdvertising();
    NimBLEAdvertisementData advertisement_data = NimBLEAdvertisementData();

    advertisement_data.setName("SBOX");
    // advertisement_data.setFlags(0x04); 
    // advertisement_data.setAppearance(0x552);
    // advertisement_data.addTxPower();

    advertising->setAdvertisementData(advertisement_data);

    advertising->setMinInterval(0xa0);
    advertising->setMaxInterval(0x1e0);

    advertising->start();
}

void ble_beacon_set_data(const std::vector<uint8_t> &data) {
    NimBLEAdvertising *advertising = NimBLEDevice::getAdvertising();

    NimBLEAdvertisementData advertisement_data = NimBLEAdvertisementData();
    advertisement_data.setName("SBOX");
    size_t first_chunk_size = std::min((size_t)18, data.size());
    advertisement_data.setManufacturerData(data.data(), first_chunk_size);
    advertising->setAdvertisementData(advertisement_data);

    if(data.size() > 18) {
    NimBLEAdvertisementData scan_response_data = NimBLEAdvertisementData();
        const uint8_t* remaining_data = &data[18];
        size_t remaining_size = data.size() - 18;
        size_t second_chunk_size = std::min((size_t)27, remaining_size);
        scan_response_data.setManufacturerData(remaining_data, second_chunk_size);
        advertising->setScanResponseData(scan_response_data);
    }
}

void ble_beacon_stop() {
    NimBLEAdvertising *advertising = NimBLEDevice::getAdvertising();

    if(advertising->isAdvertising())
        advertising->stop();
}

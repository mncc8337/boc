#ifndef BLE_H
#define BLE_H

#include <stdint.h>
#include <vector>

enum BroadcastType {
    BLE_BEACON,
    BLE_SERVER,
    WIFI,
};

extern void ble_init();

extern void ble_beacon_start();
extern void ble_beacon_set_data(const std::vector<uint8_t> &data);
extern void ble_beacon_stop();

#endif

#ifndef BLE_H
#define BLE_H

#include <stdint.h>
#include <vector>
#include <sensors.h>

enum BroadcastType {
    BLE_SERVER,
    BLE_BEACON,
    WIFI,
};

extern void ble_init();

extern void ble_beacon_start();
extern void ble_beacon_set_data(const std::vector<uint8_t> &data);
extern void make_ble_beacon_payload(const sensors_data_t &data, std::vector<uint8_t>& payload);
extern void ble_beacon_stop();

extern void ble_server_start();
extern void ble_server_update(const sensors_data_t &data, uint8_t bat_level);
extern void ble_server_stop();

#endif

#include <connectivity.h>
#include <NimBLEDevice.h>
#include <NimBLEUtils.h>

void ble_init() {
    NimBLEDevice::init("SBOX");
}

import asyncio
from enum import IntEnum
from bleak import BleakScanner

class SensorType(IntEnum):
    ACCELEROMETER = 1
    MAGNETIC_FIELD = 2
    ORIENTATION = 3
    GYROSCOPE = 4
    LIGHT = 5
    PRESSURE = 6
    PROXIMITY = 8
    GRAVITY = 9
    LINEAR_ACCELERATION = 10
    ROTATION_VECTOR = 11
    RELATIVE_HUMIDITY = 12
    AMBIENT_TEMPERATURE = 13
    OBJECT_TEMPERATURE = 14
    VOLTAGE = 15
    CURRENT = 16
    COLOR = 17
    TVOC = 18
    VOC_INDEX = 19
    NOX_INDEX = 20
    CO2 = 21
    ECO2 = 22
    PM10_STD = 23
    PM25_STD = 24
    PM100_STD = 25
    PM10_ENV = 26
    PM25_ENV = 27
    PM100_ENV = 28
    GAS_RESISTANCE = 29
    UNITLESS_PERCENT = 30
    ALTITUDE = 31

MULTIPLIER_MAP = {
    SensorType.AMBIENT_TEMPERATURE: 0.01,
    SensorType.RELATIVE_HUMIDITY: 0.01,
    SensorType.PRESSURE: 0.1,
}

UNIT_MAP = {
    SensorType.AMBIENT_TEMPERATURE: "°C",
    SensorType.RELATIVE_HUMIDITY: "%",
    SensorType.PRESSURE: "hPa",
    SensorType.LIGHT: "Lux",
    SensorType.CO2: "ppm",
}

def parse_payload(raw_data):
    i = 0
    results = {}
    
    while i < len(raw_data):
        try:
            s_type_val = raw_data[i]
            i += 1
            
            if i + 1 < len(raw_data):
                raw_val = raw_data[i] | (raw_data[i+1] << 8)
                i += 2
                
                try:
                    s_type = SensorType(s_type_val)
                    multiplier = MULTIPLIER_MAP.get(s_type, 1.0)
                    final_val = round(raw_val * multiplier, 2)
                    
                    name = s_type.name
                    unit = UNIT_MAP.get(s_type, "")
                    results[name] = f"{final_val} {unit}"
                except ValueError:
                    results[f"UNKNOWN_{s_type_val}"] = raw_val
            else:
                break
        except IndexError:
            break
            
    return results

def detection_callback(device, advertisement_data):
    if advertisement_data.local_name == "SBOX":
        all_raw = bytearray()
        sorted_m_data = sorted(advertisement_data.manufacturer_data.items())
        
        for company_id, data in sorted_m_data:
            all_raw.extend(company_id.to_bytes(2, byteorder='little'))
            all_raw.extend(data)
            
        if all_raw:
            print(f"\n[SBOX RX] Address: {device.address} | RSSI: {advertisement_data.rssi} dBm")
            print(f"RAW HEX: {all_raw.hex().upper()}")
            
            decoded = parse_payload(all_raw)
            for sensor, value in decoded.items():
                print(f"  {sensor:20}: {value}")

async def main():
    print("--- SBOX BLE BEACON RECEIVER STARTED ---")

    scanner = BleakScanner(
        detection_callback, 
        scanning_mode="active"
    )

    await scanner.start()
    try:
        while True:
            await asyncio.sleep(1.0)
    except asyncio.CancelledError:
        pass
    finally:
        await scanner.stop()

if __name__ == "__main__":
    try:
        asyncio.run(main())
    except KeyboardInterrupt:
        print("\nSBOX BLE Beacon Receiver stopped.")

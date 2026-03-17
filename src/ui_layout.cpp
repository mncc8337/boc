#include <ui_layout.h>
#include <screen.h>
#include <action.h>
#include <U8g2lib.h>
#include <sensors.h>

extern bool beacon_running;
extern bool screen_off;
extern U8G2 u8g2;

// main menu
extern OpenScreenAction open_sensor_menu;
extern OpenScreenAction open_system_menu;
extern ItemAction turn_off_screen;
extern ItemAction start_beacon;
extern ItemAction stop_beacon;

// sensor menu option
extern OpenScreenAction open_bmp280_temperature_view;
extern OpenScreenAction open_bmp280_pressure_view;
extern OpenScreenAction open_ahtx0_temperature_view;
extern OpenScreenAction open_ahtx0_humidity_view;
extern OpenScreenAction open_bh1750_view;
extern OpenScreenAction open_bmi160_acceleration_view;
extern OpenScreenAction open_bmi160_gyroscope_view;

// system menu options
extern OpenScreenAction open_connectivity_menu;
extern OpenScreenAction open_battery_menu;
extern OpenScreenAction open_beacon_menu;
extern OpenScreenAction open_info_menu;

// connectivity menu options
extern OpenScreenAction open_bluetooth_menu;
extern OpenScreenAction open_wifi_menu;

// main menu
const char *MAIN_MENU_ITEMS[] = {
    "Sensors",
    "System",
    "Turn off Screen",
    "Start Beacon",
    "Stop Beacon",
};
Action *MAIN_MENU_ITEM_ACTIONS[] = {
    &open_sensor_menu,
    &open_system_menu,
    &turn_off_screen,
    &start_beacon,
    &stop_beacon,
};
Menu main_menu(MAIN_MENU_ITEMS, MAIN_MENU_ITEM_ACTIONS, 5);

// sensor menu
const char *SENSOR_MENU_ITEMS[] = {
    "BMP280 Temp",
    "BMP280 Press",
    "AHTX0 Temp",
    "AHTX0 Humid",
    "BH1750",
    "BMI160 Accel",
    "BMI160 Gyro",
};
Action *SENSOR_MENU_ITEM_ACTIONS[] = {
    &open_bmp280_temperature_view,
    &open_bmp280_pressure_view,
    &open_ahtx0_temperature_view,
    &open_ahtx0_humidity_view,
    &open_bh1750_view,
    &open_bmi160_acceleration_view,
    &open_bmi160_gyroscope_view,
};
Menu sensor_menu(SENSOR_MENU_ITEMS, SENSOR_MENU_ITEM_ACTIONS, SENS_COUNT);
OpenScreenAction open_sensor_menu(&sensor_menu);

// bmp280 view
SensorView bmp280_temperature_view(&sensors[SENS_BMP280_TEMPERATURE]);
OpenScreenAction open_bmp280_temperature_view(&bmp280_temperature_view);
SensorView bmp280_pressure_view(&sensors[SENS_BMP280_PRESSURE]);
OpenScreenAction open_bmp280_pressure_view(&bmp280_pressure_view);

// ahtx0 view
SensorView ahtx0_temperature_view(&sensors[SENS_AHTX0_TEMPERATURE]);
OpenScreenAction open_ahtx0_temperature_view(&ahtx0_temperature_view);
SensorView ahtx0_humidity_view(&sensors[SENS_AHTX0_HUMIDITY]);
OpenScreenAction open_ahtx0_humidity_view(&ahtx0_humidity_view);

// bh1750 view
SensorView bh1750_view(&sensors[SENS_BH1750]);
OpenScreenAction open_bh1750_view(&bh1750_view);

// bmi160 view
SensorView bmi160_acceleration_view(&sensors[SENS_BMI160_ACCELERATION]);
OpenScreenAction open_bmi160_acceleration_view(&bmi160_acceleration_view);
SensorView bmi160_gyroscope_view(&sensors[SENS_BMI160_GYROSCOPE]);
OpenScreenAction open_bmi160_gyroscope_view(&bmi160_gyroscope_view);

// system menu
const char *SYSTEM_MENU_ITEMS[] = {
    "Connectivity",
    "Battery",
    "Beacon",
    "Info",
};
Action *SYSTEM_MENU_ITEM_ACTIONS[] = {
    &open_connectivity_menu,
    &open_system_menu, // &open_battery_menu,
    &open_system_menu, // &open_beacon_menu,
    &open_system_menu, // &open_info_menu,
};
Menu system_menu(SYSTEM_MENU_ITEMS, SYSTEM_MENU_ITEM_ACTIONS, 4);
OpenScreenAction open_system_menu(&system_menu);

// connectivity menu
const char *CONNECTIVITY_MENU_ITEMS[] = {
    "Bluetooth",
    "WiFi",
};
Action *CONNECTIVITY_MENU_ITEM_ACTIONS[] = {
    &open_bluetooth_menu,
    &open_wifi_menu,
};
Menu connectivity_menu(CONNECTIVITY_MENU_ITEMS, CONNECTIVITY_MENU_ITEM_ACTIONS, 2);
OpenScreenAction open_connectivity_menu(&connectivity_menu);

// bluetooth menu
const char *BLUETOOTH_MENU_ITEMS[] = {
    "blue 1",
    "blue 2",
};
Action *BLUETOOTH_MENU_ITEM_ACTIONS[] = {
    &open_bluetooth_menu,
    &open_bluetooth_menu,
};
Menu bluetooth_menu(BLUETOOTH_MENU_ITEMS, BLUETOOTH_MENU_ITEM_ACTIONS, 2);
OpenScreenAction open_bluetooth_menu(&bluetooth_menu);

// wifi menu
const char *WIFI_MENU_ITEMS[] = {
    "wifi 1",
    "wifi 2",
};
Action *WIFI_MENU_ITEM_ACTIONS[] = {
    &open_wifi_menu,
    &open_wifi_menu,
};
Menu wifi_menu(WIFI_MENU_ITEMS, WIFI_MENU_ITEM_ACTIONS, 2);
OpenScreenAction open_wifi_menu(&wifi_menu);

void itemaction_turn_off_screen() {
    if(screen_off) return;
    u8g2.setPowerSave(1);
}
ItemAction turn_off_screen(itemaction_turn_off_screen);

void itemaction_turn_on_screen() { // not really a item action, but im out of name
    if(!screen_off) return;
    u8g2.setPowerSave(0);
}

void itemaction_start_beacon_mode() {
    if(beacon_running) return;

    beacon_running = true;

    // TODO:
    // start bluetooth / beacon mode
}
ItemAction start_beacon(itemaction_start_beacon_mode);

void itemaction_stop_beacon_mode() {
    if(!beacon_running) return;
    // TODO:
    // quit bluetooth mode

    beacon_running = false;

    extern Screen *current_screen;
    current_screen->request_redraw();
}
ItemAction stop_beacon(itemaction_stop_beacon_mode);

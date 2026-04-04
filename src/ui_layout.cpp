#include <ui_layout.h>
#include <screen.h>
#include <action.h>

#include <U8g2lib.h>
#include <bitmap.h>

#include <sensors.h>

#include <connectivity.h>

#include <Preferences.h>
#include <preference_keys.h>
#include <esp_sleep.h>
#include <esp_log.h>

#include <WiFi.h>

#include <LittleFS.h>
#include <FS.h>

extern bool is_session_running;
extern bool is_datalogger_enabled;
extern unsigned long datalogger_interval;
extern bool is_telemetry_enabled;
extern U8G2 u8g2;
extern TelemetryType telemetry_type;
extern File logfile;

void toggle_session_func() {
    extern FunctionAction toggle_session;

    if(is_session_running) {
        if(is_telemetry_enabled) {
            switch(telemetry_type) {
                case BLE_BEACON: {
                    ble_beacon_stop();
                    ESP_LOGI("TELEMETRY", "BLE beacon stopped");
                    break;
                }
                case BLE_SERVER: {
                    ble_server_stop();
                    ESP_LOGI("TELEMETRY", "BLE server stopped");
                    break;
                }
                default: return; // not gonna happened
            }
        }

        if(is_datalogger_enabled && logfile) {
            logfile.close();
            ESP_LOGI("DATALOGGER", "Logfile closed");
        }

        toggle_session.name = "Start Session";
        open_notification("Session stopped");
        ESP_LOGI("SESSION", "Session stopped");
        is_session_running = false;
    } else {
        if(is_telemetry_enabled) {
            switch(telemetry_type) {
                case BLE_BEACON: {
                    ble_beacon_start();
                    puts("ble beacon started");
                    ESP_LOGI("TELEMETRY", "BLE beacon started");
                    break;
                }
                case BLE_SERVER: {
                    ble_server_start();
                    puts("ble server started");
                    ESP_LOGI("TELEMETRY", "BLE server started");
                    break;
                }
                default: {
                    open_notification("Unsupported\telemetry type");
                    return;
                }
            }
        }

        if(is_datalogger_enabled && !logfile) {
            logfile = LittleFS.open("/data.log", FILE_APPEND);
            extern bool force_file_flush;
            force_file_flush = true;
            ESP_LOGI("DATALOGGER", "Logfile opened");
        }

        if(is_telemetry_enabled || is_datalogger_enabled) {
            toggle_session.name = "Stop Session";
            open_notification("Session started");
            ESP_LOGI("SESSION", "Session started");
            is_session_running = true;

            // set polling rate
            if(is_datalogger_enabled && !is_telemetry_enabled) {
                set_session_data_sampling_interval(datalogger_interval);
            } else if(!is_datalogger_enabled && is_telemetry_enabled) {
                set_session_data_sampling_interval(100);
            } else {
                // if both enabled then set to the smaller one
                if(100 < datalogger_interval) {
                    set_session_data_sampling_interval(100);
                } else {
                    set_session_data_sampling_interval(datalogger_interval);
                }
            }
        } else {
            open_notification("No job to do\nPlease turn on\neither telemetry\nor datalogger");
        }
    }
}
FunctionAction toggle_session("Start Session", toggle_session_func);

DummyAction ble_server_option("BLE Server");
DummyAction ble_beacon_option("BLE Beacon");
DummyAction wifi_option("WiFi");
std::vector<DummyAction*> connectivity_menu_items = {
    &ble_server_option,
    &ble_beacon_option,
    &wifi_option
};
std::vector<TelemetryType> connectivity_menu_item_map = {
    BLE_SERVER,
    BLE_BEACON,
    WIFI,
};
void connectivity_callback(TelemetryType nval) {
    Preferences pref;
    pref.begin(PREFERENCES, false);
    pref.putUChar(KEY_TELEMETRY_TYPE, nval);
    pref.end();
    ESP_LOGI("TELEMETRY", "Set connectivity to %d", nval);
}
RadioMenu<TelemetryType> connectivity_menu(
    connectivity_menu_items,
    telemetry_type,
    connectivity_menu_item_map,
    connectivity_callback
);
OpenScreenAction open_connectivity_menu("Connectivity", &connectivity_menu);

DummyAction telemetry_state_menu_item_disable("Disable");
DummyAction telemetry_state_menu_item_enable("Enable");
std::vector<DummyAction*> telemetry_state_menu_items = {
    &telemetry_state_menu_item_disable,
    &telemetry_state_menu_item_enable,
};
std::vector<bool> telemetry_state_menu_item_map = {false, true};
void telemetry_state_callback(bool nval) {
    Preferences pref;
    pref.begin(PREFERENCES, false);
    pref.putBool(KEY_TELEMETRY_ENABLE, nval);
    pref.end();
    ESP_LOGI("TELEMETRY", "State set to %d", nval);
}
RadioMenu<bool> telemetry_state_menu(
    telemetry_state_menu_items,
    is_telemetry_enabled,
    telemetry_state_menu_item_map,
    telemetry_state_callback,
    &is_session_running
);
OpenScreenAction open_telemetry_state_menu("Broadcasting", &telemetry_state_menu);

std::vector<Action*> telemetry_menu_items = {
    &open_connectivity_menu,
    &open_telemetry_state_menu
};
Menu telemetry_menu(telemetry_menu_items, &is_session_running);
OpenScreenAction open_telemetry_menu("Telemetry", &telemetry_menu);

DummyAction datalogger_interval_menu_item_1s("1s");
DummyAction datalogger_interval_menu_item_30s("30s");
DummyAction datalogger_interval_menu_item_1m("1m");
DummyAction datalogger_interval_menu_item_5m("5m");
DummyAction datalogger_interval_menu_item_10m("10m");
DummyAction datalogger_interval_menu_item_30m("30m");
DummyAction datalogger_interval_menu_item_1h("1h");
DummyAction datalogger_interval_menu_item_2h("2h");
DummyAction datalogger_interval_menu_item_6h("6h");
DummyAction datalogger_interval_menu_item_12h("12h");
DummyAction datalogger_interval_menu_item_1d("1d");
std::vector<DummyAction*> datalogger_interval_menu_items = {
    &datalogger_interval_menu_item_1s,
    &datalogger_interval_menu_item_30s,
    &datalogger_interval_menu_item_1m,
    &datalogger_interval_menu_item_5m,
    &datalogger_interval_menu_item_10m,
    &datalogger_interval_menu_item_30m,
    &datalogger_interval_menu_item_1h,
    &datalogger_interval_menu_item_2h,
    &datalogger_interval_menu_item_6h,
    &datalogger_interval_menu_item_12h,
    &datalogger_interval_menu_item_1d,
};
std::vector<unsigned long> datalogger_interval_menu_item_map = {
    1000,
    30 * 1000,
    60 * 1000,
    5 * 60 * 1000,
    10 * 60 * 1000,
    30 * 60 * 1000,
    60 * 60 * 1000,
    2 * 60 * 60 * 1000,
    6 * 60 * 60 * 1000,
    12 * 60 * 60 * 1000,
    24 * 60 * 60 * 1000,
};
void datalogger_interval_callback(unsigned long nval) {
    Preferences pref;
    pref.begin(PREFERENCES, false);
    pref.putULong(KEY_DATALOGGER_INTERVAL, nval);
    pref.end();
    ESP_LOGI("DATALOGGER", "Interval set to %dms", nval);
}
RadioMenu<unsigned long> datalogger_interval_menu(
    datalogger_interval_menu_items,
    datalogger_interval,
    datalogger_interval_menu_item_map,
    datalogger_interval_callback,
    &is_session_running
);
OpenScreenAction open_datalogger_interval_menu("Interval", &datalogger_interval_menu);

DummyAction datalogger_state_menu_item_disable("Disable");
DummyAction datalogger_state_menu_item_enable("Enable");
std::vector<DummyAction*> datalogger_state_menu_items = {
    &datalogger_state_menu_item_disable,
    &datalogger_state_menu_item_enable,
};
std::vector<bool> datalogger_state_menu_item_map = {false, true};
void datalogger_state_callback(bool nval) {
    Preferences pref;
    pref.begin(PREFERENCES, false);
    pref.putBool(KEY_DATALOGGER_ENABLE, nval);
    pref.end();
    ESP_LOGI("DATALOGGER", "State set to %d", nval);
}
RadioMenu<bool> datalogger_state_menu(
    datalogger_state_menu_items,
    is_datalogger_enabled,
    datalogger_state_menu_item_map,
    datalogger_state_callback,
    &is_session_running
);
OpenScreenAction open_datalogger_state_menu("Logging", &datalogger_state_menu);

std::vector<Action*> datalogger_menu_items = {
    &open_datalogger_interval_menu,
    &open_datalogger_state_menu,
};
Menu datalogger_menu(datalogger_menu_items, &is_session_running);
OpenScreenAction open_datalogger_menu("Datalogger", &datalogger_menu);

DummyAction open_sensor_config_menu("Config");

extern sensor_mask_t lognsend_mask;
std::vector<DummyAction*> lognsend_menu_items = {};
std::vector<unsigned> lognsend_menu_bit_map = {};
void lognsend_callback(sensor_mask_t nmask) {
    ESP_LOGD("SENSORS", "New log & send mask: 0x%X", nmask);
}
CheckBoxMenu<sensor_mask_t> lognsend_menu(
    lognsend_menu_items,
    lognsend_mask,
    lognsend_menu_bit_map,
    lognsend_callback,
    &is_session_running
);
OpenScreenAction open_lognsend_menu("Log & Send", &lognsend_menu);

std::vector<Action*> sensors_menu_items = {
    &open_sensor_config_menu,
    &open_lognsend_menu,
};
Menu sensors_menu(sensors_menu_items, &is_session_running);
OpenScreenAction open_sensors_menu("Sensors", &sensors_menu);

extern uint8_t screen_brightness;
DummyAction screen_brightness_menu_item_10("10");
DummyAction screen_brightness_menu_item_25("25");
DummyAction screen_brightness_menu_item_50("50");
DummyAction screen_brightness_menu_item_75("75");
DummyAction screen_brightness_menu_item_100("100");
std::vector<DummyAction*> screen_brightness_menu_items = {
    &screen_brightness_menu_item_10,
    &screen_brightness_menu_item_25,
    &screen_brightness_menu_item_50,
    &screen_brightness_menu_item_75,
    &screen_brightness_menu_item_100,
};
std::vector<uint8_t> screen_brightness_menu_item_map = {8, 26, 64, 128, 255};
void brightness_callback(uint8_t nval) {
    Preferences pref;
    pref.begin(PREFERENCES, false);
    pref.putUChar(KEY_SCREEN_BRIGHTNESS, nval);
    pref.end();
    u8g2.setContrast(nval);
    ESP_LOGI("SCREEN", "Brightness set to %d", nval);
}
RadioMenu<uint8_t> screen_brightness_menu(
    screen_brightness_menu_items,
    screen_brightness,
    screen_brightness_menu_item_map,
    brightness_callback
);
OpenScreenAction open_screen_brightness_menu("Brightness", &screen_brightness_menu);

extern unsigned long screen_timeout;
DummyAction screen_timeout_menu_item_30s("30s");
DummyAction screen_timeout_menu_item_3m("3m");
DummyAction screen_timeout_menu_item_5m("5m");
DummyAction screen_timeout_menu_item_10m("10m");
std::vector<DummyAction*> screen_timeout_menu_items = {
    &screen_timeout_menu_item_30s,
    &screen_timeout_menu_item_3m,
    &screen_timeout_menu_item_5m,
    &screen_timeout_menu_item_10m,
};
std::vector<unsigned long> screen_timeout_menu_item_map = {
    30000,
    3 * 60000,
    5 * 60000,
    10 * 60000,
};
void screen_timeout_callback(unsigned long nval) {
    Preferences pref;
    pref.begin(PREFERENCES, false);
    pref.putULong(KEY_SCREEN_TIMEOUT, nval);
    pref.end();
    ESP_LOGI("SCREEN", "Timeout set to %d", nval);
}
RadioMenu<unsigned long> screen_timeout_menu(
    screen_timeout_menu_items,
    screen_timeout,
    screen_timeout_menu_item_map,
    screen_timeout_callback
);
OpenScreenAction open_screen_timeout_menu("Screen Timeout", &screen_timeout_menu);

void invert_screen_color() {
    static bool screen_inverted = false;

    if(!screen_inverted)
        u8g2.sendF("c", 0xA7);
    else
        u8g2.sendF("c", 0xA6);

    screen_inverted = !screen_inverted;
    ESP_LOGI("SCREEN", "Color inverted");
}
FunctionAction screen_invert("Invert Color", invert_screen_color);

std::vector<Action*> screen_menu_items = {
    &open_screen_brightness_menu,
    &open_screen_timeout_menu,
    &screen_invert,
};
Menu screen_menu(screen_menu_items);
OpenScreenAction open_screen_menu("Screen", &screen_menu);

void do_sync_time() {
    if(is_session_running) {
        open_notification("Please stop\nthe current session\nto sync time");
        return;
    }

    open_notification("Trying to connect\nto WiFi\n" WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    unsigned tries = 0;
    while(WiFi.status() != WL_CONNECTED && tries < 10) {
        delay(500);
        tries++;
    }
    if(WiFi.status() != WL_CONNECTED) {
        open_notification("Failed to\nconnect to\nWiFi");
        ESP_LOGE("Time", "Failed to connect to WiFi \"" WIFI_SSID "\"");
        WiFi.disconnect(true);
        WiFi.mode(WIFI_OFF);
        return;
    }
    open_notification("Syncing ...");

    configTzTime(TZ_INFO, NTP_SERVER);

    struct tm timeinfo;
    getLocalTime(&timeinfo);

    open_notification("Time synced");
    ESP_LOGI("TIME", "Time synced");

    WiFi.disconnect(true); 
    WiFi.mode(WIFI_OFF);
}
FunctionAction sync_time("Sync Time", do_sync_time);

void do_clear_datalog() {
    if(LittleFS.exists("/data.log")) {
        File f = LittleFS.open("/data.log", "w");
        f.close();
        open_notification("Log file cleared");
        ESP_LOGI("DATALOGGER", "Log file cleared");
    } else {
        open_notification("File does not\nexisted");
    }
}
FunctionAction clear_datalog("Clear datalog", do_clear_datalog);

FunctionAction reboot("Reboot", esp_restart);

extern void shutdown();
FunctionAction do_shutdown("Shutdown", shutdown);

std::vector<Action*> settings_menu_items = {
    &open_telemetry_menu,
    &open_datalogger_menu,
    &open_sensors_menu,
    &open_screen_menu,
    &sync_time,
    &clear_datalog,
    &reboot,
    &do_shutdown,
};
Menu settings_menu(settings_menu_items);
OpenScreenAction open_settings_menu("Settings", &settings_menu);

std::vector<Action*> live_data_menu_items;
Menu live_data_menu(live_data_menu_items);
OpenScreenAction open_live_data_menu("Live Data", &live_data_menu);

void do_open_webserver() {
    extern bool is_webserver_running;
    extern FunctionAction open_webserver;
    if(is_webserver_running) {
        extern void end_webserver();
        end_webserver();
        open_notification("Server ended");
        open_webserver.name = "Start Server";
    } else {
        extern void setup_webserver();
        setup_webserver();
        IPAddress ip = WiFi.localIP();
        open_notification(("Server started at\n" + std::string(ip.toString().c_str())));
        open_webserver.name = "Stop Server";
    }
}
FunctionAction open_webserver("Start Server", do_open_webserver);

InfoScreen info_screen_instance;
OpenScreenAction open_info_menu("Info", &info_screen_instance);

SplashScreen splash_screen;
OpenScreenAction open_splash_screen("Splash GIF", &splash_screen);

std::vector<Action*> tools_menu_items = {
    &open_live_data_menu,
    &open_webserver,
    &open_info_menu,
    &open_splash_screen,
};
Menu tools_menu(tools_menu_items);
OpenScreenAction open_tools_menu("Tools", &tools_menu);

std::vector<Action*> main_menu_items = {
    &toggle_session,
    &open_settings_menu,
    &open_tools_menu,
};
Menu main_menu(main_menu_items);

void ui_init() {
    live_data_menu_items.reserve(SENS_COUNT);
    for(unsigned i = 0; i < SENS_COUNT; i++) {
        if(!SENSOR_ALIVE(i)) continue;

        sensor_t sensor_info;
        sensors[i]->getSensor(&sensor_info);

        SensorView *sensor_view = new SensorView(i, sensor_info);
        OpenScreenAction *open_sensor_view = new OpenScreenAction(
            std::string(sensor_info.name),
            sensor_view
        );

        live_data_menu_items.push_back(open_sensor_view);

        lognsend_menu_items.push_back(new DummyAction(sensor_info.name));
        lognsend_menu_bit_map.push_back(i);
    }
}

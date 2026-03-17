#include <Arduino.h>
#include <esp_sleep.h>

// GUI
#include <U8g2lib.h>
#include <screen.h>
#include <action.h>

// sensors
#include <Adafruit_Sensor.h>
#include <BH1750_US.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_AHTX0.h>
#include <BMI160_US.h>

#define SDA_PIN 8
#define SCL_PIN 9

#define BUTTON_UP_PIN 3
#define BUTTON_DOWN_PIN 1
#define BUTTON_SELECT_PIN 4

bool button_up_clicked = false;
bool button_select_clicked = false;
bool button_down_clicked = false;
unsigned long first_select_press_ts = 0;

Adafruit_BMP280 bmp280;
Adafruit_AHTX0 ahtx0;
BH1750 bh1750_hw;
BH1750_US bh1750(bh1750_hw, 100);
BMI160_US_Accelerometer bmi160_accel(103);
BMI160_US_Gyroscope bmi160_gyro(104);

Adafruit_Sensor *sensors[] = {
    bmp280.getPressureSensor(),
    ahtx0.getTemperatureSensor(),
    ahtx0.getHumiditySensor(),
    &bh1750,
    &bmi160_accel,
    &bmi160_gyro,
};
const unsigned sensor_count = 6;

Screen *current_screen;
Screen *screen_stack[8];
unsigned screen_stack_ptr = 0;
extern void open_screen(Screen *screen) {
    if(screen_stack_ptr >= 8 || screen_stack[screen_stack_ptr - 1] == current_screen)
        return;
    screen_stack[screen_stack_ptr++] = current_screen;
    current_screen = screen;
    current_screen->request_redraw();
}
void open_prev_screen() {
    if(!screen_stack_ptr) return;
    current_screen = screen_stack[--screen_stack_ptr];
    current_screen->request_redraw();
}

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0);

extern OpenScreenAction open_sensor_menu;
extern OpenScreenAction open_system_menu;
extern ItemAction start_beacon_mode;

// sensor menu option
extern OpenScreenAction open_bmp280_view;
extern OpenScreenAction open_ahtx0_view;
// extern OpenScreenAction open_bh1750_view;
// extern OpenScreenAction open_bmi160_view;

// system menu options
extern OpenScreenAction open_connectivity_menu;
extern OpenScreenAction open_sensor_status_menu;

// connectivity menu options
extern OpenScreenAction open_bluetooth_menu;
extern OpenScreenAction open_wifi_menu;

// main menu
const char *MAIN_MENU_ITEMS[] = {
    "Sensors",
    "System",
    "Start Beacon",
};
Action *MAIN_MENU_ITEM_ACTIONS[] = {
    &open_sensor_menu,
    &open_system_menu,
    &start_beacon_mode
};
Menu main_menu(MAIN_MENU_ITEMS, MAIN_MENU_ITEM_ACTIONS, 3);

// sensor menu
const char *SENSOR_MENU_ITEMS[] = {
    "BMP280",
    "AHTX0",
    // "BH1750",
    // "BMI160",
};
Action *SENSOR_MENU_ITEM_ACTIONS[] = {
    &open_bmp280_view,
    &open_ahtx0_view,
    // &open_bh1750_view,
    // &open_bmi160_view,
};
Menu sensor_menu(SENSOR_MENU_ITEMS, SENSOR_MENU_ITEM_ACTIONS, 2);
OpenScreenAction open_sensor_menu(&sensor_menu);

// bmp280 view
BMP280View bmp280_view(bmp280);
OpenScreenAction open_bmp280_view(&bmp280_view);

// ahtx0 view
AHTX0View ahtx0_view(ahtx0);
OpenScreenAction open_ahtx0_view(&ahtx0_view);

// // bh1750 view
// SensorView bh1750_view();
// OpenScreenAction open_bh1750_view(&bh1750_view);
//
// // bmi160 view
// SensorView bmi160_view();
// OpenScreenAction open_bmi160_view(&bmi160_view);

// system menu
const char *SYSTEM_MENU_ITEMS[] = {
    "Connectivity",
    "Sensor status",
};
Action *SYSTEM_MENU_ITEM_ACTIONS[] = {
    &open_connectivity_menu,
    &open_sensor_status_menu,
};
Menu system_menu(SYSTEM_MENU_ITEMS, SYSTEM_MENU_ITEM_ACTIONS, 2);
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

// sensor status menu
const char *SENSOR_STATUS_MENU_ITEMS[] = {
    "sensor 1",
    "sensor 2",
};
Action *SENSOR_STATUS_MENU_ITEM_ACTIONS[] = {
    &open_sensor_status_menu,
    &open_sensor_status_menu,
};
Menu sensor_status_menu(SENSOR_STATUS_MENU_ITEMS, SENSOR_STATUS_MENU_ITEM_ACTIONS, 2);
OpenScreenAction open_sensor_status_menu(&sensor_status_menu);

void itemaction_start_beacon_mode() {
    while(digitalRead(BUTTON_SELECT_PIN) == HIGH) {
        delay(10);
    }

    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_squeezed_b7_tr);
    u8g2.drawStr(7, 7, "press start again");
    u8g2.drawStr(7, 16, "to quit this mode");
    u8g2.drawStr(7, 25, "press ok to continue");
    u8g2.sendBuffer();
    while(digitalRead(BUTTON_SELECT_PIN) == LOW) {
        delay(10);
    }
    delay(100);

    u8g2.setPowerSave(1);
    bh1750_hw.configure(BH1750::ONE_TIME_HIGH_RES_MODE_2);

    setCpuFrequencyMhz(80);

    // TODO:
    // start bluetooth / beacon mode

    while(digitalRead(BUTTON_SELECT_PIN) == LOW) {
        // TODO
        // read sensors + send packets
        delay(100); 
    }

    // TODO:
    // quit bluetooth mode

    setCpuFrequencyMhz(160);
    bh1750_hw.configure(BH1750::CONTINUOUS_HIGH_RES_MODE_2);
    u8g2.setPowerSave(0);

    while(digitalRead(BUTTON_SELECT_PIN) == HIGH) {
        delay(10); 
    }

    current_screen->request_redraw();

    // uint64_t wakeup_pin_mask = 1ULL << BUTTON_SELECT_PIN;
    // esp_deep_sleep_enable_gpio_wakeup(wakeup_pin_mask, ESP_GPIO_WAKEUP_GPIO_HIGH);
    // esp_deep_sleep_start();

    // gpio_wakeup_enable((gpio_num_t)BUTTON_SELECT_PIN, GPIO_INTR_HIGH_LEVEL);
    // esp_sleep_enable_gpio_wakeup();
    // esp_light_sleep_start();
}
ItemAction start_beacon_mode(itemaction_start_beacon_mode);

void setup() {
    pinMode(BUTTON_UP_PIN, INPUT);
    pinMode(BUTTON_DOWN_PIN, INPUT);
    pinMode(BUTTON_SELECT_PIN, INPUT);

    bool init_ok = true;

    Wire.begin();

    u8g2.setColorIndex(1);
    u8g2.begin();
    u8g2.setBitmapMode(1);

    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_4x6_tr);

    int current_y = 6;
    if(!bh1750_hw.begin(BH1750::CONTINUOUS_HIGH_RES_MODE_2, 0x23)) {
        init_ok = 0;
        u8g2.drawStr(0, current_y, "failed to init BH1750");
        current_y += 6 + 2;
    }

    if(!ahtx0.begin()) {
        init_ok = 0;
        u8g2.drawStr(0, current_y, "failed to init AHTX0");
        current_y += 6 + 2;
    }

    if(!bmp280.begin(0x77)) {
        init_ok = 0;
        u8g2.drawStr(0, current_y, "failed to init BMP280");
        current_y += 6 + 2;
    }

    if(!BMI160.begin(BMI160GenClass::I2C_MODE, 0x69)) {
        init_ok = 0;
        u8g2.drawStr(0, current_y, "failed to init BMI160");
        current_y += 6 + 2;
    } else {
        BMI160.setFullScaleAccelRange(BMI160_ACCEL_RANGE_4G); 
        BMI160.setGyroRange(1000);
    }

    if(!init_ok) {
        u8g2.drawStr(0, current_y, "failed to init some peripherals");
        current_y += 6 + 2;
        u8g2.drawStr(0, current_y, "proceed without those?");
        u8g2.sendBuffer();
        while(digitalRead(BUTTON_SELECT_PIN) == LOW);
    }

    current_screen = &main_menu;
    current_screen->request_redraw();
}

void loop() {
    int button_select_state = digitalRead(BUTTON_SELECT_PIN);
    int button_up_state = digitalRead(BUTTON_UP_PIN);
    int button_down_state = digitalRead(BUTTON_DOWN_PIN);

    unsigned long button_select_press_time = 0;
    bool button_up_action = false;
    bool button_down_action = false;

    if(button_select_state == HIGH && !button_select_clicked) {
        button_select_clicked = true;
        first_select_press_ts = millis();
    }
    if(button_select_state == LOW && button_select_clicked) { 
        button_select_press_time = millis() - first_select_press_ts;
        button_select_clicked = false;
    }

    if(button_up_state == HIGH && !button_up_clicked) {
        button_up_clicked = true;
    }
    if(button_up_state == LOW && button_up_clicked) {
        button_up_clicked = false;
        button_up_action = true;
    }

    if(button_down_state == HIGH && !button_down_clicked) {
        button_down_clicked = true;
    } 
    if(button_down_state == LOW && button_down_clicked) {
        button_down_clicked = false;
        button_down_action = true;
    }

    if(button_select_press_time >= 400) {
        // global action
        open_prev_screen();
    } else {
        current_screen->process_navigation(
            button_select_press_time,
            button_up_action,
            button_down_action
        );

        current_screen->draw(u8g2);
    }
}

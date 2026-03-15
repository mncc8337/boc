#include <Arduino.h>
#include <U8g2lib.h>
#include <screen.h>

#define BUTTON_UP_PIN 3
#define BUTTON_DOWN_PIN 1
#define BUTTON_SELECT_PIN 4

int button_up_clicked = 0;
int button_select_clicked = 0;
int button_down_clicked = 0;

const char *MAIN_MENU_ITEMS[] = {
    "sensors view",
    "enable list",
    "system",
    "sleep",
};

Menu main_menu(MAIN_MENU_ITEMS, 4);
const unsigned MAIN_MENU_ITEM_COUNT = 4;

const char *SENSOR_MENU_ITEMS[] = {
    "BMP280",
    "AHT20",
    "BH1750",
    "BMI160",
    "<- back"
};
Menu sensor_menu(SENSOR_MENU_ITEMS, 5);
const unsigned SENSOR_MENU_ITEM_COUNT = 5;

Screen *MAIN_MENU_ITEM_ACTIONS[] = {
    &sensor_menu,
    &main_menu,
    &main_menu,
    &main_menu
};

Screen *SENSOR_MENU_ITEM_ACTIONS[] = {
    &sensor_menu,
    &sensor_menu,
    &sensor_menu,
    &sensor_menu,
    &main_menu
};

Screen *current_screen = &main_menu;
Screen *prev_screen = NULL;

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0);

void setup() {
    u8g2.setColorIndex(1);
    u8g2.begin();
    u8g2.setBitmapMode(1);

    pinMode(BUTTON_UP_PIN, INPUT);
    pinMode(BUTTON_DOWN_PIN, INPUT);
    pinMode(BUTTON_SELECT_PIN, INPUT);

    main_menu.set_item_actions(MAIN_MENU_ITEM_ACTIONS);
    sensor_menu.set_item_actions(SENSOR_MENU_ITEM_ACTIONS);
}

void loop() {
    current_screen->process_navigation(
        BUTTON_UP_PIN,
        BUTTON_DOWN_PIN,
        button_up_clicked,
        button_down_clicked
    );

    current_screen->draw(u8g2);

    if((digitalRead(BUTTON_SELECT_PIN) == HIGH) && (button_select_clicked == 0)) {
        button_select_clicked = 1;
        if(!current_screen->is_menu()) {
            current_screen = prev_screen;
        } else {
            prev_screen = current_screen;
            current_screen = current_screen->get_item_action();
        }
    }
    if((digitalRead(BUTTON_SELECT_PIN) == LOW) && (button_select_clicked == 1)) { 
        button_select_clicked = 0;
    }
}

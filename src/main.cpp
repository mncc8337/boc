#include <Arduino.h>
#include <Wire.h>
#include <esp_sleep.h>

#include <sensors.h>
#include <ui_layout.h>

#define SDA_PIN 8
#define SCL_PIN 9

#define BUTTON_UP_PIN 3
#define BUTTON_DOWN_PIN 1
#define BUTTON_SELECT_PIN 4

bool button_up_clicked = false;
bool button_select_clicked = false;
bool button_down_clicked = false;
unsigned long first_select_press_ts = 0;

bool beacon_running = false;
bool screen_off = false;

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0);

Screen *current_screen;
Screen *screen_stack[8];
unsigned screen_stack_ptr = 0;
void open_screen(Screen *screen) {
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

void setup() {
    pinMode(BUTTON_UP_PIN, INPUT);
    pinMode(BUTTON_DOWN_PIN, INPUT);
    pinMode(BUTTON_SELECT_PIN, INPUT);

    bool init_ok = true;

    Wire.begin(SDA_PIN, SCL_PIN);

    u8g2.setColorIndex(1);
    u8g2.begin();
    u8g2.setBitmapMode(1);

    if(!init_sensors()) {
        u8g2.printf("failed to init all sensors");
        while(1);
    }

    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_4x6_tr);

    current_screen = &main_menu;
    current_screen->request_redraw();
}

void loop() {
    int button_select_state = digitalRead(BUTTON_SELECT_PIN);
    int button_up_state = digitalRead(BUTTON_UP_PIN);
    int button_down_state = digitalRead(BUTTON_DOWN_PIN);

    unsigned long button_select_press_time = 0;
    bool button_up_action = screen_off; // lock these buttons when screen is off
    bool button_down_action = screen_off;

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
        button_up_action ^= true;
    }

    if(button_down_state == HIGH && !button_down_clicked) {
        button_down_clicked = true;
    } 
    if(button_down_state == LOW && button_down_clicked) {
        button_down_clicked = false;
        button_down_action ^= true;
    }

    if(button_select_press_time >= 400) {
        // global action
        open_prev_screen();
    } else {
        if(screen_off) itemaction_turn_on_screen();

        current_screen->process_navigation(
            button_select_press_time,
            button_up_action,
            button_down_action
        );

        current_screen->draw(u8g2);
    }
}

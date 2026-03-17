#include <screen.h>
#include <action.h>
#include <bitmap.h>
#include <Adafruit_AHTX0.h>

AHTX0View::AHTX0View(Adafruit_AHTX0 &sensor) : bmp280(sensor) {};

void AHTX0View::process_navigation(
    unsigned long button_select_press_time,
    bool button_up_clicked,
    bool button_down_clicked
) {};

void AHTX0View::draw(U8G2 &u8g2) {
    if(!redraw_request) return;

    u8g2.clearBuffer();

    u8g2.setFont(u8g2_font_7x14_tr);
    u8g2.drawStr(0, 14, "ahtx0");

    u8g2.sendBuffer();

    redraw_request = false;
}

bool AHTX0View::is_menu() {
    return false;
}


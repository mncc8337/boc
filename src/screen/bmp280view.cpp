#include <screen.h>
#include <action.h>
#include <bitmap.h>
#include <Adafruit_BMP280.h>

BMP280View::BMP280View(Adafruit_BMP280 &sensor) : bmp280(sensor) {};

void BMP280View::process_navigation(
    unsigned long button_select_press_time,
    bool button_up_clicked,
    bool button_down_clicked
) {};

void BMP280View::draw(U8G2 &u8g2) {
    if(!redraw_request) return;

    u8g2.clearBuffer();

    u8g2.setFont(u8g2_font_7x14_tr);
    u8g2.drawStr(0, 14, "bmp280");

    u8g2.sendBuffer();

    redraw_request = false;
}

bool BMP280View::is_menu() {
    return false;
}

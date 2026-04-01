#include <cstring>
#include <screen.h>
#include <action.h>
#include <bitmap.h>

extern void open_prev_screen();
extern void open_screen(Screen *screen, bool forced);

// global notification object
static Notification notif;

void Notification::set_message(std::string new_message) {
    message = new_message;
    request_redraw();
}

void Notification::process_navigation(
    unsigned long button_select_press_duration,
    bool button_up_clicked,
    bool button_down_clicked
) {
    if(button_select_press_duration > 50 || button_up_clicked || button_down_clicked) {
        open_prev_screen();
    }
}

void Notification::draw(U8G2 &u8g2) {
    u8g2.drawXBM(10, 10, 108, 44, BITMAP_NOTIFICATION_BOX);

    u8g2.setDrawColor(0);
    u8g2.drawBox(11, 11, 108 - 3, 44 - 3);
    u8g2.setDrawColor(1);

    u8g2.setFont(u8g2_font_6x10_tf);

    int current_y = 15 + 7;
    int current_x = 15;
    int start_pos = 0;
    size_t end_pos = message.find('\n');
    while(end_pos != std::string::npos) {
        u8g2.setCursor(current_x, current_y);
        u8g2.print(message.substr(start_pos, end_pos - start_pos).c_str());
        current_x = 15;
        current_y += 7 + 2;
        start_pos = end_pos + 1;
        end_pos = message.find('\n', start_pos);
    }
    u8g2.setCursor(current_x, current_y);
    u8g2.print(message.substr(start_pos).c_str());

    redraw_request = false;
}

bool Notification::is_overlay() {
    return true;
}

void open_notification(std::string message) {
    notif.set_message(message);
    open_screen(&notif, true);
}

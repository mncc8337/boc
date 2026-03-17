#include <screen.h>
#include <bitmap.h>

bool Screen::is_menu() {
    return false;
}

void Screen::request_redraw() {
    redraw_request = true;
}

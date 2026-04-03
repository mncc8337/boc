#include <screen.h>
#include <action.h>
#include <bitmap.h>
#include <connectivity.h>

template <typename T>
RadioMenu<T>::RadioMenu(
    std::vector<DummyAction*> &items,
    T &bound_target,
    std::vector<T> &value_map,
    void (*callback)(T new_value),
    bool *block_flag
)
    :   Menu((std::vector<Action*> &)items, block_flag),
        bound_target(bound_target),
        value_map(value_map),
        callback(callback) {}

template <typename T>
void RadioMenu<T>::process_navigation(
    unsigned long button_select_press_duration,
    bool button_up_clicked,
    bool button_down_clicked
) {
    Menu::process_navigation(
        button_select_press_duration,
        button_up_clicked,
        button_down_clicked
    );

    if(button_select_press_duration > 50) {
        radio_state = item_selected;
        request_redraw();
    }
}

template <typename T>
void RadioMenu<T>::draw(U8G2 &u8g2) {
    Menu::draw(u8g2);

    if(radio_state >= item_sel_previous && radio_state <= item_sel_next) {
        u8g2.drawXBM(4, 2 + 22 * (radio_state - item_sel_previous), 16, 16, BITMAP_RADIODOT);
    }
}

template <typename T>
void RadioMenu<T>::open_callback() {
    for(radio_state = 0; radio_state < items.size(); radio_state++) {
        if(value_map[radio_state] == bound_target) break;
    }

    if(radio_state == items.size()) {
        radio_state = 0;
    }

    item_selected = radio_state;
    item_sel_previous = item_selected - 1;
    item_sel_next = item_selected + 1;

    for(unsigned i = 0; i < items.size(); i++) {
        items[i]->icon = BITMAP_RADIOBUTTON;
    }
}

template <typename T>
void RadioMenu<T>::close_callback() {
    bound_target = value_map[radio_state];
    if(callback) callback(bound_target);
}

template class RadioMenu<int>;
template class RadioMenu<uint8_t>;
template class RadioMenu<unsigned long>;
template class RadioMenu<bool>;
template class RadioMenu<TelemetryType>;

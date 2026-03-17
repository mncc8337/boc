#ifndef SCREEN_H
#define SCREEN_H

#include <U8g2lib.h>

class Action;
class Adafruit_Sensor;

class Screen {
protected:
    bool redraw_request = false;

public:
    virtual void draw(U8G2 &u8g2) = 0;
    virtual void process_navigation(
        unsigned long button_select_press_time,
        bool button_up_clicked,
        bool button_down_clicked
    ) = 0;
    virtual bool is_menu();
    void request_redraw();
};

class SensorView: public Screen {
private:
    Adafruit_Sensor **sensor_ptr;

    char sensor_name[32];
    int32_t sensor_type;
    bool initialized = false;
public:
    SensorView(Adafruit_Sensor **sensor_ptr);
    void process_navigation(
        unsigned long button_select_press_time,
        bool button_up_clicked,
        bool button_down_clicked
    ) override;
    void draw(U8G2 &u8g2) override;
    bool is_menu() override;
};

class Menu: public Screen {
private:
    const char **menu_items;
    Action **item_actions;
    unsigned item_count;

    int item_selected = 0;
    int item_sel_previous;
    int item_sel_next;

public:
    Menu(const char **_menu_items, Action **_item_actions, unsigned _item_count);
    void process_navigation(
        unsigned long button_select_press_time,
        bool button_up_clicked,
        bool button_down_clicked
    ) override;
    void draw(U8G2 &u8g2) override;
    bool is_menu() override;

    Action *get_item_action();
};

#endif

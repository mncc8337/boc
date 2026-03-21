#include <screen.h>
#include <sensors.h>
#include <esp_mac.h>
#include <esp_system.h>
#include <esp_task_wdt.h>

extern uint8_t battery_percentage;
extern uint16_t psu_voltage_avg;
extern bool broadcasting;

void InfoScreen::process_navigation(
    unsigned long button_select_press_duration,
    bool button_up_clicked,
    bool button_down_clicked
) {
    if(button_down_clicked) {
        current_y -= 8;
        redraw_request = true;
    }

    if(button_up_clicked && current_y < 6) {
        current_y += 8;
        redraw_request = true;
    }

    if(millis() - last_update_ts >= 1000) {
        redraw_request = true;
        last_update_ts = millis();
    }
}

void InfoScreen::draw(U8G2 &u8g2) {
    u8g2.setFont(u8g2_font_5x8_tf);

    int y = current_y;

    u8g2.setCursor(0, y); u8g2.printf("Battery: %dmV | %d%%", psu_voltage_avg, battery_percentage);
    y += 8;

    uint32_t free_heap = esp_get_free_heap_size() / 1024;
    u8g2.setCursor(0, y); u8g2.printf("Free mem: %luKiB", free_heap);
    y += 8;

    uint64_t uptime_sec = esp_timer_get_time() / 1000000;
    u8g2.setCursor(0, y); u8g2.printf("Uptime: %llus", uptime_sec);
    y += 8;

    uint8_t mac[6];
    esp_read_mac(mac, ESP_MAC_WIFI_STA); 
    u8g2.setCursor(0, y); 
    u8g2.printf("MAC: %02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    y += 8;

    int alive_count = 0;
    for(unsigned i = 0; i < SENS_COUNT; i++) {
        if(SENSOR_ALIVE(i)) alive_count++;
    }
    u8g2.setCursor(0, y); 
    u8g2.printf("Sensors OK: %d/%d", alive_count, SENS_COUNT);
    y += 8;

    u8g2.setCursor(0, y);
    u8g2.print("RST Reason: ");
    switch(esp_reset_reason()) {
        case ESP_RST_UNKNOWN:
            u8g2.print("UNKNOWN");
            break;
        case ESP_RST_POWERON:
            u8g2.print("PWR_ON");
            break;
        case ESP_RST_EXT:
            u8g2.print("EXT_PIN");
            break;
        case ESP_RST_SW:
            u8g2.print("SOFT");
            break;
        case ESP_RST_PANIC:
            u8g2.print("PANIC");
            break;
        case ESP_RST_INT_WDT:
            u8g2.print("INT_WDT");
            break;
        case ESP_RST_TASK_WDT:
            u8g2.print("TSK_WDT");
            break;
        case ESP_RST_WDT:
            u8g2.print("OTHER_WDT");
            break;
        case ESP_RST_DEEPSLEEP:
            u8g2.print("WAKE_DS");
            break;
        case ESP_RST_BROWNOUT:
            u8g2.print("BROWNOUT");
            break;
        case ESP_RST_SDIO:
            u8g2.print("SDIO");
            break;
        default:
            u8g2.print("OTHER");
    }
    y += 8;

    u8g2.setCursor(0, y);
    y += 8;
    u8g2.print("Build:");

    u8g2.setCursor(5, y);
    u8g2.printf("%s %s", __DATE__, __TIME__);
    y += 8;

    redraw_request = false;
}

#ifndef BITMAP_H
#define BITMAP_H

#include <Arduino.h>

// 'scrollbar_background', 8x64px
extern const uint8_t BITMAP_SCROLLBAR_BACKGROUND[] PROGMEM;
// 'item_sel_outline', 124x20px
extern const uint8_t BITMAP_ITEM_SEL_OUTLINE[] PROGMEM;

// 'radiobutton', 16x16px
extern const uint8_t BITMAP_RADIOBUTTON [] PROGMEM;
// 'radiodot', 16x16px
extern const uint8_t BITMAP_RADIODOT [] PROGMEM;
// 'check', 16x16px
extern const uint8_t BITMAP_CHECK[] PROGMEM;
// 'checkbox', 16x16px
extern const uint8_t BITMAP_CHECKBOX[] PROGMEM;

// 'notification-box', 108x44px
extern const uint8_t BITMAP_NOTIFICATION_BOX[] PROGMEM;

extern const unsigned BITMAP_SPLASH_LEN;
extern const uint8_t *BITMAP_SPLASH[];

#endif

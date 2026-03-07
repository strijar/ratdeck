#include "LvInput.h"
#include "hal/Keyboard.h"
#include "hal/Trackball.h"
#include "hal/TouchInput.h"

namespace LvInput {

static Keyboard* s_kb = nullptr;
static Trackball* s_tb = nullptr;
static TouchInput* s_touch = nullptr;
static lv_group_t* s_group = nullptr;

// Keypad indev state
static uint32_t s_lastKey = 0;
static lv_indev_state_t s_keyState = LV_INDEV_STATE_RELEASED;
static bool s_keyReady = false;

// Touch disabled — GT911 coordinate mapping needs calibration

static void keypad_read_cb(lv_indev_drv_t* drv, lv_indev_data_t* data) {
    if (s_keyReady) {
        data->key = s_lastKey;
        data->state = s_keyState;
        s_keyReady = false;
    } else {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}

void init(Keyboard* kb, Trackball* tb, TouchInput* touch) {
    s_kb = kb;
    s_tb = tb;
    s_touch = touch;

    // Create input group
    s_group = lv_group_create();
    lv_group_set_default(s_group);

    // Register keypad indev
    static lv_indev_drv_t keyDrv;
    lv_indev_drv_init(&keyDrv);
    keyDrv.type = LV_INDEV_TYPE_KEYPAD;
    keyDrv.read_cb = keypad_read_cb;
    lv_indev_t* keyIndev = lv_indev_drv_register(&keyDrv);
    lv_indev_set_group(keyIndev, s_group);

    // Touch indev disabled — GT911 coordinate mapping needs calibration
    Serial.println("[LVGL] Input drivers registered (touch disabled)");
}

void feedKey(const KeyEvent& evt) {
    uint32_t key = 0;

    if (evt.enter) key = LV_KEY_ENTER;
    else if (evt.up) key = LV_KEY_UP;
    else if (evt.down) key = LV_KEY_DOWN;
    else if (evt.left) key = LV_KEY_LEFT;
    else if (evt.right) key = LV_KEY_RIGHT;
    else if (evt.del) key = LV_KEY_BACKSPACE;
    else if (evt.tab) key = LV_KEY_NEXT;
    else if (evt.character == 0x1B) key = LV_KEY_ESC;
    else return;  // Don't feed printable chars — screens handle them via handleKey()

    s_lastKey = key;
    s_keyState = LV_INDEV_STATE_PRESSED;
    s_keyReady = true;
}

lv_group_t* group() {
    return s_group;
}

}  // namespace LvInput

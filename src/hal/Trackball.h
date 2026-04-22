#pragma once

#include <Arduino.h>
#include "config/BoardConfig.h"

class Trackball {
public:
    bool begin();

    // Poll GPIO state changes
    void update();

    // Cursor position
    bool isPressed();
    bool movedLeft();
    bool movedRight();
    bool movedUp();
    bool movedDown();

    // Speed multiplier (1-5)
    void setSpeed(uint8_t speed) { _speed = constrain(speed, 1, 5); }

private:
    static void IRAM_ATTR isrUp();
    static void IRAM_ATTR isrDown();
    static void IRAM_ATTR isrLeft();
    static void IRAM_ATTR isrRight();

    uint8_t _speed = 3;

    static volatile int8_t _deltaX;
    static volatile int8_t _deltaY;
    static Trackball* _instance;
};

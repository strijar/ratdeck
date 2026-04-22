#include "Trackball.h"

volatile int8_t Trackball::_deltaX = 0;
volatile int8_t Trackball::_deltaY = 0;
Trackball* Trackball::_instance = nullptr;

bool Trackball::begin() {
    _instance = this;

    // Configure trackball GPIOs as inputs with pullup
    pinMode(TBALL_UP, INPUT_PULLUP);
    pinMode(TBALL_DOWN, INPUT_PULLUP);
    pinMode(TBALL_LEFT, INPUT_PULLUP);
    pinMode(TBALL_RIGHT, INPUT_PULLUP);

    // Attach interrupts for movement detection
    attachInterrupt(digitalPinToInterrupt(TBALL_UP), isrUp, FALLING);
    attachInterrupt(digitalPinToInterrupt(TBALL_DOWN), isrRight, FALLING);   // Physical down pin = rightward
    attachInterrupt(digitalPinToInterrupt(TBALL_LEFT), isrLeft, FALLING);
    attachInterrupt(digitalPinToInterrupt(TBALL_RIGHT), isrDown, FALLING);   // Physical right pin = downward

    Serial.println("[TRACKBALL] Initialized");
    return true;
}

bool Trackball::isPressed() {
    return (digitalRead(TBALL_CLICK) == LOW);
}

bool Trackball::movedLeft() {
    bool res = false;

    noInterrupts();
    if (_deltaX < -_speed) {
        _deltaX += _speed;
        res = true;
    }
    interrupts();

    return res;
}

bool Trackball::movedRight() {
    bool res = false;

    noInterrupts();
    if (_deltaX > _speed) {
        _deltaX -= _speed;
        res = true;
    }
    interrupts();

    return res;
}

bool Trackball::movedUp() {
    bool res = false;

    noInterrupts();
    if (_deltaY < -_speed) {
        _deltaY += _speed;
        res = true;
    }
    interrupts();

    return res;
}

bool Trackball::movedDown() {
    bool res = false;

    noInterrupts();
    if (_deltaY > _speed) {
        _deltaY -= _speed;
        res = true;
    }
    interrupts();

    return res;
}

void Trackball::update() {
}

void IRAM_ATTR Trackball::isrUp()    { _deltaY--; }
void IRAM_ATTR Trackball::isrDown()  { _deltaY++; }
void IRAM_ATTR Trackball::isrLeft()  { _deltaX--; }
void IRAM_ATTR Trackball::isrRight() { _deltaX++; }

#include "InputManager.h"
#include "config/BoardConfig.h"

void InputManager::begin(Keyboard* kb, Trackball* tb, TouchInput* touch) {
    _kb = kb;
    _tb = tb;
    _touch = touch;
}

void InputManager::update() {
    _hasKey = false;
    _activity = false;
    _strongActivity = false;
    _longPress = false;

    // Poll keyboard
    if (_kb) {
        _kb->update();
        if (_kb->hasEvent()) {
            _keyEvent = _kb->getEvent();
            _hasKey = true;
            _activity = true;
            _strongActivity = true;
        }
    }

    // Touch activity check — throttled to ~50Hz
    if (_touch) {
        unsigned long now = millis();
        if (now - _lastTouchPoll >= TOUCH_POLL_MS) {
            _lastTouchPoll = now;
            _touch->update();
            if (_touch->isTouched()) {
                _activity = true;
                _strongActivity = true;
            }
        }
    }
}

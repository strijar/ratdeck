#include "LvStatusBar.h"
#include "Theme.h"
#include <Arduino.h>

void LvStatusBar::create(lv_obj_t* parent) {
    _bar = lv_obj_create(parent);
    lv_obj_set_size(_bar, Theme::SCREEN_W, Theme::STATUS_BAR_H);
    lv_obj_align(_bar, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_set_style_bg_color(_bar, lv_color_hex(Theme::BG), 0);
    lv_obj_set_style_bg_opa(_bar, LV_OPA_COVER, 0);
    lv_obj_set_style_border_color(_bar, lv_color_hex(Theme::BORDER), 0);
    lv_obj_set_style_border_width(_bar, 1, 0);
    lv_obj_set_style_border_side(_bar, LV_BORDER_SIDE_BOTTOM, 0);
    lv_obj_set_style_pad_all(_bar, 0, 0);
    lv_obj_set_style_radius(_bar, 0, 0);
    lv_obj_clear_flag(_bar, LV_OBJ_FLAG_SCROLLABLE);

    const lv_font_t* font = &lv_font_montserrat_12;

    // Left side: LoRa BLE WiFi indicators
    _lblLora = lv_label_create(_bar);
    lv_obj_set_style_text_font(_lblLora, font, 0);
    lv_label_set_text(_lblLora, "LoRa");
    lv_obj_align(_lblLora, LV_ALIGN_LEFT_MID, 4, 0);

    _lblBle = lv_label_create(_bar);
    lv_obj_set_style_text_font(_lblBle, font, 0);
    lv_label_set_text(_lblBle, "BLE");
    lv_obj_align(_lblBle, LV_ALIGN_LEFT_MID, 50, 0);

    _lblWifi = lv_label_create(_bar);
    lv_obj_set_style_text_font(_lblWifi, font, 0);
    lv_label_set_text(_lblWifi, "WiFi");
    lv_obj_align(_lblWifi, LV_ALIGN_LEFT_MID, 84, 0);

    // Center: "Ratspeak"
    _lblBrand = lv_label_create(_bar);
    lv_obj_set_style_text_font(_lblBrand, font, 0);
    lv_obj_set_style_text_color(_lblBrand, lv_color_hex(Theme::ACCENT), 0);
    lv_label_set_text(_lblBrand, "Ratspeak");
    lv_obj_align(_lblBrand, LV_ALIGN_CENTER, 0, 0);

    // Right: Battery %
    _lblBatt = lv_label_create(_bar);
    lv_obj_set_style_text_font(_lblBatt, font, 0);
    lv_label_set_text(_lblBatt, "");
    lv_obj_align(_lblBatt, LV_ALIGN_RIGHT_MID, -4, 0);

    // Toast overlay (hidden by default)
    _toast = lv_obj_create(parent);
    lv_obj_set_size(_toast, Theme::SCREEN_W, Theme::STATUS_BAR_H);
    lv_obj_align(_toast, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_set_style_bg_color(_toast, lv_color_hex(Theme::ACCENT), 0);
    lv_obj_set_style_bg_opa(_toast, LV_OPA_COVER, 0);
    lv_obj_set_style_pad_all(_toast, 0, 0);
    lv_obj_set_style_radius(_toast, 0, 0);
    lv_obj_clear_flag(_toast, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(_toast, LV_OBJ_FLAG_HIDDEN);

    _lblToast = lv_label_create(_toast);
    lv_obj_set_style_text_font(_lblToast, font, 0);
    lv_obj_set_style_text_color(_lblToast, lv_color_hex(Theme::BG), 0);
    lv_obj_center(_lblToast);
    lv_label_set_text(_lblToast, "");

    // Set initial indicator colors
    refreshIndicators();
}

void LvStatusBar::update() {
    // Handle announce flash timeout
    if (_announceFlashEnd > 0 && millis() >= _announceFlashEnd) {
        _announceFlashEnd = 0;
        refreshIndicators();
    }

    // Handle toast timeout
    if (_toastEnd > 0 && millis() >= _toastEnd) {
        _toastEnd = 0;
        lv_obj_add_flag(_toast, LV_OBJ_FLAG_HIDDEN);
    }
}

void LvStatusBar::setLoRaOnline(bool online) {
    _loraOnline = online;
    refreshIndicators();
}

void LvStatusBar::setBLEActive(bool active) {
    _bleActive = active;
    refreshIndicators();
}

void LvStatusBar::setWiFiActive(bool active) {
    _wifiActive = active;
    refreshIndicators();
}

void LvStatusBar::setBatteryPercent(int pct) {
    if (_battPct == pct) return;
    _battPct = pct;
    if (pct >= 0) {
        char buf[8];
        snprintf(buf, sizeof(buf), "%d%%", pct);
        lv_label_set_text(_lblBatt, buf);
        uint32_t col = Theme::PRIMARY;
        if (pct <= 15) col = Theme::ERROR_CLR;
        else if (pct <= 30) col = Theme::WARNING_CLR;
        lv_obj_set_style_text_color(_lblBatt, lv_color_hex(col), 0);
    }
}

void LvStatusBar::setTransportMode(const char* mode) {
    (void)mode;
}

void LvStatusBar::flashAnnounce() {
    _announceFlashEnd = millis() + 1000;
    refreshIndicators();
}

void LvStatusBar::showToast(const char* msg, uint32_t durationMs) {
    lv_label_set_text(_lblToast, msg);
    _toastEnd = millis() + durationMs;
    lv_obj_clear_flag(_toast, LV_OBJ_FLAG_HIDDEN);
}

void LvStatusBar::refreshIndicators() {
    bool flashing = _announceFlashEnd > 0 && millis() < _announceFlashEnd;

    // LoRa: green=online, red=offline, cyan if TX flash
    if (flashing) {
        lv_obj_set_style_text_color(_lblLora, lv_color_hex(Theme::ACCENT), 0);
    } else if (_loraOnline) {
        lv_obj_set_style_text_color(_lblLora, lv_color_hex(Theme::PRIMARY), 0);
    } else {
        lv_obj_set_style_text_color(_lblLora, lv_color_hex(Theme::ERROR_CLR), 0);
    }

    // BLE: green=active, yellow=enabled-not-connected, red=disabled
    if (_bleActive) {
        lv_obj_set_style_text_color(_lblBle, lv_color_hex(Theme::PRIMARY), 0);
    } else if (_bleEnabled) {
        lv_obj_set_style_text_color(_lblBle, lv_color_hex(Theme::WARNING_CLR), 0);
    } else {
        lv_obj_set_style_text_color(_lblBle, lv_color_hex(Theme::ERROR_CLR), 0);
    }

    // WiFi: green=connected, yellow=enabled-not-connected, red=disabled
    if (_wifiActive) {
        lv_obj_set_style_text_color(_lblWifi, lv_color_hex(Theme::PRIMARY), 0);
    } else if (_wifiEnabled) {
        lv_obj_set_style_text_color(_lblWifi, lv_color_hex(Theme::WARNING_CLR), 0);
    } else {
        lv_obj_set_style_text_color(_lblWifi, lv_color_hex(Theme::ERROR_CLR), 0);
    }
}

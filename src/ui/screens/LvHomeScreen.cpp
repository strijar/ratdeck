#include "LvHomeScreen.h"
#include "ui/Theme.h"
#include "reticulum/ReticulumManager.h"
#include "radio/SX1262.h"
#include "config/UserConfig.h"
#include <Arduino.h>
#include <esp_system.h>

void LvHomeScreen::createUI(lv_obj_t* parent) {
    _screen = parent;
    lv_obj_set_layout(parent, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(parent, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_all(parent, 6, 0);
    lv_obj_set_style_pad_row(parent, 4, 0);
    lv_obj_clear_flag(parent, LV_OBJ_FLAG_SCROLLABLE);

    const lv_font_t* font = &lv_font_montserrat_14;
    auto mkLabel = [&](const char* initial) -> lv_obj_t* {
        lv_obj_t* lbl = lv_label_create(parent);
        lv_obj_set_style_text_font(lbl, font, 0);
        lv_obj_set_style_text_color(lbl, lv_color_hex(Theme::PRIMARY), 0);
        lv_label_set_text(lbl, initial);
        return lbl;
    };

    _lblId = mkLabel("Identity: ...");
    _lblTransport = mkLabel("Transport: ...");
    _lblPaths = mkLabel("Paths: ...");
    _lblLora = mkLabel("Radio: ...");
    _lblHeap = mkLabel("Heap: ...");
    _lblPsram = mkLabel("PSRAM: ...");
    _lblUptime = mkLabel("Uptime: 0m");

    // Force refresh on new UI — invalidate cache
    _lastUptime = ULONG_MAX;
    _lastHeap = UINT32_MAX;
    refreshUI();
}

void LvHomeScreen::onEnter() {
    // Invalidate cache so refreshUI() always updates after screen transition
    _lastUptime = ULONG_MAX;
    _lastHeap = UINT32_MAX;
    refreshUI();
}

void LvHomeScreen::refreshUI() {
    if (!_lblId) return;

    unsigned long upMins = millis() / 60000;
    uint32_t heap = ESP.getFreeHeap() / 1024;
    if (upMins == _lastUptime && heap == _lastHeap) return;
    _lastUptime = upMins;
    _lastHeap = heap;

    if (_rns) {
        lv_label_set_text_fmt(_lblId, "ID: %s", _rns->identityHash().c_str());
        lv_label_set_text_fmt(_lblTransport, "Transport: %s",
            _rns->isTransportActive() ? "ACTIVE" : "OFFLINE");
        lv_label_set_text_fmt(_lblPaths, "Paths: %d  Links: %d",
            (int)_rns->pathCount(), (int)_rns->linkCount());
    } else {
        lv_label_set_text(_lblId, "Identity: ---");
        lv_label_set_text(_lblTransport, "Transport: OFFLINE");
        lv_label_set_text(_lblPaths, "Paths: 0  Links: 0");
    }

    if (_radio && _radio->isRadioOnline()) {
        lv_label_set_text_fmt(_lblLora, "LoRa: SF%d BW%luk %ddBm",
            _radio->getSpreadingFactor(),
            (unsigned long)(_radio->getSignalBandwidth() / 1000),
            _radio->getTxPower());
        lv_obj_set_style_text_color(_lblLora, lv_color_hex(Theme::PRIMARY), 0);
    } else {
        lv_label_set_text(_lblLora, "Radio: OFFLINE");
        lv_obj_set_style_text_color(_lblLora, lv_color_hex(Theme::ERROR_CLR), 0);
    }

    lv_label_set_text_fmt(_lblHeap, "Heap: %lukB free",
        (unsigned long)(ESP.getFreeHeap() / 1024));
    lv_label_set_text_fmt(_lblPsram, "PSRAM: %lukB free",
        (unsigned long)(ESP.getFreePsram() / 1024));

    if (upMins >= 60) {
        lv_label_set_text_fmt(_lblUptime, "Uptime: %luh %lum", upMins / 60, upMins % 60);
    } else {
        lv_label_set_text_fmt(_lblUptime, "Uptime: %lum", upMins);
    }
}

bool LvHomeScreen::handleKey(const KeyEvent& event) {
    if (event.enter || event.character == '\n' || event.character == '\r') {
        if (_announceCb) _announceCb();
        return true;
    }
    return false;
}

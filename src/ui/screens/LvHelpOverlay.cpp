#include "LvHelpOverlay.h"
#include "ui/Theme.h"
#include <lvgl.h>

void LvHelpOverlay::create() {
    _overlay = lv_obj_create(lv_layer_top());
    lv_obj_set_size(_overlay, Theme::CONTENT_W - 40, Theme::CONTENT_H - 20);
    lv_obj_center(_overlay);
    lv_obj_set_style_bg_color(_overlay, lv_color_hex(Theme::BG), 0);
    lv_obj_set_style_bg_opa(_overlay, 240, 0);
    lv_obj_set_style_border_color(_overlay, lv_color_hex(Theme::PRIMARY), 0);
    lv_obj_set_style_border_width(_overlay, 1, 0);
    lv_obj_set_style_radius(_overlay, 4, 0);
    lv_obj_set_style_pad_all(_overlay, 10, 0);
    lv_obj_set_style_pad_row(_overlay, 3, 0);
    lv_obj_set_layout(_overlay, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(_overlay, LV_FLEX_FLOW_COLUMN);

    const lv_font_t* font = &lv_font_montserrat_12;

    auto mkLabel = [&](const char* text, uint32_t color) {
        lv_obj_t* lbl = lv_label_create(_overlay);
        lv_obj_set_style_text_font(lbl, font, 0);
        lv_obj_set_style_text_color(lbl, lv_color_hex(color), 0);
        lv_label_set_text(lbl, text);
    };

    mkLabel("HOTKEYS", Theme::ACCENT);
    mkLabel("Ctrl+H  Help", Theme::PRIMARY);
    mkLabel("Ctrl+M  Messages", Theme::PRIMARY);
    mkLabel("Ctrl+N  New Message", Theme::PRIMARY);
    mkLabel("Ctrl+S  Settings", Theme::PRIMARY);
    mkLabel("Ctrl+A  Announce", Theme::PRIMARY);
    mkLabel("Ctrl+D  Diagnostics", Theme::PRIMARY);
    mkLabel("Ctrl+T  Radio Test", Theme::PRIMARY);
    mkLabel("Ctrl+R  RSSI Monitor", Theme::PRIMARY);
    mkLabel(",  /    Prev/Next Tab", Theme::PRIMARY);
    mkLabel("Esc     Back", Theme::PRIMARY);
    mkLabel("", Theme::MUTED);
    mkLabel("Any key to close", Theme::MUTED);

    lv_obj_add_flag(_overlay, LV_OBJ_FLAG_HIDDEN);
}

void LvHelpOverlay::show() {
    if (!_overlay) create();
    lv_obj_clear_flag(_overlay, LV_OBJ_FLAG_HIDDEN);
    _visible = true;
}

void LvHelpOverlay::hide() {
    if (_overlay) lv_obj_add_flag(_overlay, LV_OBJ_FLAG_HIDDEN);
    _visible = false;
}

void LvHelpOverlay::toggle() {
    if (_visible) hide(); else show();
}

bool LvHelpOverlay::handleKey(const KeyEvent& event) {
    if (_visible) {
        hide();
        return true;
    }
    return false;
}

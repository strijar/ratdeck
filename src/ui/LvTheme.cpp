#include "LvTheme.h"
#include "Theme.h"
#include <Arduino.h>

namespace LvTheme {

static lv_style_t s_screen;
static lv_style_t s_label;
static lv_style_t s_labelMuted;
static lv_style_t s_labelAccent;
static lv_style_t s_btn;
static lv_style_t s_btnPressed;
static lv_style_t s_bar;
static lv_style_t s_barIndicator;
static lv_style_t s_switch;
static lv_style_t s_switchChecked;
static lv_style_t s_textarea;
static lv_style_t s_list;
static lv_style_t s_listBtn;
static lv_style_t s_listBtnFocused;
static lv_style_t s_dropdown;
static lv_style_t s_slider;

void init(lv_disp_t* disp) {
    // Screen background
    lv_style_init(&s_screen);
    lv_style_set_bg_color(&s_screen, lv_color_hex(Theme::BG));
    lv_style_set_bg_opa(&s_screen, LV_OPA_COVER);
    lv_style_set_text_color(&s_screen, lv_color_hex(Theme::PRIMARY));
    lv_style_set_text_font(&s_screen, &lv_font_montserrat_14);

    // Labels
    lv_style_init(&s_label);
    lv_style_set_text_color(&s_label, lv_color_hex(Theme::PRIMARY));

    lv_style_init(&s_labelMuted);
    lv_style_set_text_color(&s_labelMuted, lv_color_hex(Theme::MUTED));

    lv_style_init(&s_labelAccent);
    lv_style_set_text_color(&s_labelAccent, lv_color_hex(Theme::ACCENT));

    // Buttons
    lv_style_init(&s_btn);
    lv_style_set_bg_color(&s_btn, lv_color_hex(Theme::BG));
    lv_style_set_bg_opa(&s_btn, LV_OPA_COVER);
    lv_style_set_border_color(&s_btn, lv_color_hex(Theme::BORDER));
    lv_style_set_border_width(&s_btn, 1);
    lv_style_set_text_color(&s_btn, lv_color_hex(Theme::PRIMARY));
    lv_style_set_radius(&s_btn, 3);
    lv_style_set_pad_all(&s_btn, 6);

    lv_style_init(&s_btnPressed);
    lv_style_set_bg_color(&s_btnPressed, lv_color_hex(Theme::SELECTION_BG));
    lv_style_set_border_color(&s_btnPressed, lv_color_hex(Theme::PRIMARY));

    // Bar
    lv_style_init(&s_bar);
    lv_style_set_bg_color(&s_bar, lv_color_hex(Theme::BORDER));
    lv_style_set_bg_opa(&s_bar, LV_OPA_COVER);
    lv_style_set_radius(&s_bar, 2);

    lv_style_init(&s_barIndicator);
    lv_style_set_bg_color(&s_barIndicator, lv_color_hex(Theme::PRIMARY));
    lv_style_set_bg_opa(&s_barIndicator, LV_OPA_COVER);
    lv_style_set_radius(&s_barIndicator, 2);

    // Switch
    lv_style_init(&s_switch);
    lv_style_set_bg_color(&s_switch, lv_color_hex(Theme::MUTED));
    lv_style_set_bg_opa(&s_switch, LV_OPA_COVER);
    lv_style_set_radius(&s_switch, LV_RADIUS_CIRCLE);

    lv_style_init(&s_switchChecked);
    lv_style_set_bg_color(&s_switchChecked, lv_color_hex(Theme::PRIMARY));

    // Textarea
    lv_style_init(&s_textarea);
    lv_style_set_bg_color(&s_textarea, lv_color_hex(Theme::BG));
    lv_style_set_bg_opa(&s_textarea, LV_OPA_COVER);
    lv_style_set_border_color(&s_textarea, lv_color_hex(Theme::BORDER));
    lv_style_set_border_width(&s_textarea, 1);
    lv_style_set_text_color(&s_textarea, lv_color_hex(Theme::PRIMARY));
    lv_style_set_radius(&s_textarea, 2);
    lv_style_set_pad_all(&s_textarea, 6);

    // List
    lv_style_init(&s_list);
    lv_style_set_bg_color(&s_list, lv_color_hex(Theme::BG));
    lv_style_set_bg_opa(&s_list, LV_OPA_COVER);
    lv_style_set_pad_all(&s_list, 0);
    lv_style_set_pad_row(&s_list, 0);
    lv_style_set_border_width(&s_list, 0);

    lv_style_init(&s_listBtn);
    lv_style_set_bg_color(&s_listBtn, lv_color_hex(Theme::BG));
    lv_style_set_bg_opa(&s_listBtn, LV_OPA_COVER);
    lv_style_set_text_color(&s_listBtn, lv_color_hex(Theme::PRIMARY));
    lv_style_set_border_color(&s_listBtn, lv_color_hex(Theme::BORDER));
    lv_style_set_border_width(&s_listBtn, 0);
    lv_style_set_border_side(&s_listBtn, LV_BORDER_SIDE_BOTTOM);
    lv_style_set_pad_all(&s_listBtn, 5);
    lv_style_set_radius(&s_listBtn, 0);

    lv_style_init(&s_listBtnFocused);
    lv_style_set_bg_color(&s_listBtnFocused, lv_color_hex(Theme::SELECTION_BG));
    lv_style_set_border_color(&s_listBtnFocused, lv_color_hex(Theme::PRIMARY));
    lv_style_set_border_width(&s_listBtnFocused, 1);

    // Dropdown
    lv_style_init(&s_dropdown);
    lv_style_set_bg_color(&s_dropdown, lv_color_hex(Theme::BG));
    lv_style_set_bg_opa(&s_dropdown, LV_OPA_COVER);
    lv_style_set_border_color(&s_dropdown, lv_color_hex(Theme::BORDER));
    lv_style_set_border_width(&s_dropdown, 1);
    lv_style_set_text_color(&s_dropdown, lv_color_hex(Theme::PRIMARY));
    lv_style_set_radius(&s_dropdown, 2);
    lv_style_set_pad_all(&s_dropdown, 4);

    // Slider
    lv_style_init(&s_slider);
    lv_style_set_bg_color(&s_slider, lv_color_hex(Theme::BORDER));
    lv_style_set_bg_opa(&s_slider, LV_OPA_COVER);

    // Apply screen style to default theme
    lv_obj_add_style(lv_scr_act(), &s_screen, 0);

    Serial.println("[LVGL] Ratspeak theme initialized");
}

lv_style_t* styleScreen()         { return &s_screen; }
lv_style_t* styleLabel()          { return &s_label; }
lv_style_t* styleLabelMuted()     { return &s_labelMuted; }
lv_style_t* styleLabelAccent()    { return &s_labelAccent; }
lv_style_t* styleBtn()            { return &s_btn; }
lv_style_t* styleBtnPressed()     { return &s_btnPressed; }
lv_style_t* styleBar()            { return &s_bar; }
lv_style_t* styleBarIndicator()   { return &s_barIndicator; }
lv_style_t* styleSwitch()         { return &s_switch; }
lv_style_t* styleSwitchChecked()  { return &s_switchChecked; }
lv_style_t* styleTextarea()       { return &s_textarea; }
lv_style_t* styleList()           { return &s_list; }
lv_style_t* styleListBtn()        { return &s_listBtn; }
lv_style_t* styleListBtnFocused() { return &s_listBtnFocused; }
lv_style_t* styleDropdown()       { return &s_dropdown; }
lv_style_t* styleSlider()         { return &s_slider; }

}  // namespace LvTheme

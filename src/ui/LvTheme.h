#pragma once

#include <lvgl.h>

// Ratspeak LVGL theme — matrix green on black cyberpunk aesthetic
namespace LvTheme {

void init(lv_disp_t* disp);

// Style accessors for common elements
lv_style_t* styleScreen();
lv_style_t* styleLabel();
lv_style_t* styleLabelMuted();
lv_style_t* styleLabelAccent();
lv_style_t* styleBtn();
lv_style_t* styleBtnPressed();
lv_style_t* styleBar();
lv_style_t* styleBarIndicator();
lv_style_t* styleSwitch();
lv_style_t* styleSwitchChecked();
lv_style_t* styleTextarea();
lv_style_t* styleList();
lv_style_t* styleListBtn();
lv_style_t* styleListBtnFocused();
lv_style_t* styleDropdown();
lv_style_t* styleSlider();

}  // namespace LvTheme

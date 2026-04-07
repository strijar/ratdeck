#include "LvTabView.h"
#include "UIManager.h"
#include "Theme.h"
#include "LvTheme.h"
#include <cstdio>
#include "fonts/fonts.h"

void LvTabView::create(lv_obj_t* parent) {
    _bar = lv_tabview_create(parent, LV_DIR_BOTTOM, Theme::TAB_BAR_H);
    lv_obj_set_size(_bar, Theme::SCREEN_W, Theme::SCREEN_H - Theme::STATUS_BAR_H);
    lv_obj_set_y(_bar, Theme::STATUS_BAR_H);

    lv_obj_t * tab_btns = lv_tabview_get_tab_btns(_bar);

    lv_obj_set_style_text_font(tab_btns, &lv_font_ratdeck_12, 0);

    lv_obj_set_style_bg_color(tab_btns, lv_color_hex(Theme::BG), LV_PART_ITEMS);
    lv_obj_set_style_bg_opa(tab_btns, LV_OPA_COVER, LV_PART_ITEMS);
    lv_obj_set_style_border_color(tab_btns, lv_color_hex(Theme::DIVIDER), LV_PART_ITEMS);
    lv_obj_set_style_border_side(tab_btns, LV_BORDER_SIDE_TOP, LV_PART_ITEMS);
    lv_obj_set_style_border_width(tab_btns, 1, LV_PART_ITEMS);

    lv_obj_set_style_bg_color(tab_btns, lv_color_hex(Theme::BG_HOVER), LV_PART_ITEMS | LV_STATE_CHECKED);
}

void LvTabView::addScreen(LvScreen *screen) {
    lv_obj_t *obj = lv_tabview_add_tab(_bar, screen->title());

    screen->createUI(obj);
}

void LvTabView::setUnreadCount(int tab, int count) {
}

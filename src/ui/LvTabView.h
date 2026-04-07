#pragma once

#include <lvgl.h>

class LvScreen;

class LvTabView {
public:
    enum Tab { TAB_HOME = 0, TAB_CONTACTS, TAB_MSGS, TAB_NODES, TAB_SETTINGS, TAB_COUNT = 5 };

    void create(lv_obj_t* parent);
    void addScreen(LvScreen *screen);

    void setUnreadCount(int tab, int count);

    lv_obj_t* obj() { return _bar; }

private:
    lv_obj_t* _bar = nullptr;
};

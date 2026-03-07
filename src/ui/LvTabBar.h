#pragma once

#include <lvgl.h>

class LvTabBar {
public:
    enum Tab { TAB_HOME = 0, TAB_MSGS, TAB_NODES, TAB_SETUP, TAB_COUNT = 4 };

    void create(lv_obj_t* parent);

    void setActiveTab(int tab);
    int getActiveTab() const { return _activeTab; }
    void cycleTab(int direction);

    void setUnreadCount(int tab, int count);

    using TabCallback = void(*)(int tab);
    void setTabCallback(TabCallback cb) { _tabCb = cb; }

    lv_obj_t* obj() { return _bar; }

private:
    void refreshTabs();

    lv_obj_t* _bar = nullptr;
    lv_obj_t* _tabs[TAB_COUNT] = {};
    int _activeTab = TAB_HOME;
    int _unread[TAB_COUNT] = {};
    TabCallback _tabCb = nullptr;

    static constexpr const char* TAB_NAMES[TAB_COUNT] = {"Home", "Msgs", "Nodes", "Setup"};
};

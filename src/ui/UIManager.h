#pragma once

#include <lvgl.h>
#include "LvStatusBar.h"
#include "LvTabBar.h"
#include "StatusBar.h"
#include "TabBar.h"
#include "hal/Keyboard.h"

class LGFX_TDeck;

// Legacy screen base class (LovyanGFX direct drawing)
class Screen {
public:
    virtual ~Screen() = default;
    virtual void onEnter() {}
    virtual void onExit() {}
    virtual void update() {}
    virtual bool handleKey(const KeyEvent& event) { return false; }
    virtual const char* title() const = 0;
    virtual void draw(LGFX_TDeck& gfx) = 0;

    bool isDirty() const { return _dirty; }
    void markDirty() { _dirty = true; }
    void clearDirty() { _dirty = false; }

protected:
    bool _dirty = true;
};

// LVGL screen base class
class LvScreen {
public:
    virtual ~LvScreen() = default;
    virtual void createUI(lv_obj_t* parent) = 0;
    virtual void destroyUI();
    virtual void refreshUI() {}
    virtual void onEnter() {}
    virtual void onExit() {}
    virtual bool handleKey(const KeyEvent& event) { return false; }
    virtual bool handleLongPress() { return false; }
    virtual const char* title() const = 0;

    lv_obj_t* screen() const { return _screen; }

protected:
    lv_obj_t* _screen = nullptr;
};

class UIManager {
public:
    void begin(LGFX_TDeck* gfx);

    // Legacy screen management (LovyanGFX)
    void setScreen(Screen* screen);
    Screen* getScreen() { return _currentScreen; }

    // LVGL screen management
    void setLvScreen(LvScreen* screen);
    LvScreen* getLvScreen() { return _currentLvScreen; }

    // Component access — legacy (for old code compatibility)
    StatusBar& statusBar() { return _statusBar; }
    TabBar& tabBar() { return _tabBar; }

    // Component access — LVGL
    LvStatusBar& lvStatusBar() { return _lvStatusBar; }
    LvTabBar& lvTabBar() { return _lvTabBar; }

    // Update data (called periodically)
    void update();

    // Render if dirty (called from loop — legacy screens only)
    void render();

    // Force full redraw
    void forceRedraw();

    // Handle key event — routes to current screen
    bool handleKey(const KeyEvent& event);
    bool handleLongPress();

    // Boot mode — hides status bar and tab bar
    void setBootMode(bool boot);
    bool isBootMode() const { return _bootMode; }

    // Overlay support (legacy)
    void setOverlay(Screen* overlay);

    // LVGL content area parent (between status bar and tab bar)
    lv_obj_t* contentParent() { return _lvContent; }

private:
    LGFX_TDeck* _gfx = nullptr;

    // Legacy components
    StatusBar _statusBar;
    TabBar _tabBar;
    Screen* _currentScreen = nullptr;
    Screen* _overlay = nullptr;

    // LVGL components
    LvStatusBar _lvStatusBar;
    LvTabBar _lvTabBar;
    LvScreen* _currentLvScreen = nullptr;
    lv_obj_t* _lvContent = nullptr;

    bool _bootMode = false;
    bool _lvglActive = false;
};

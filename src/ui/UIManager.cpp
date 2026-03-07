#include "UIManager.h"
#include "Theme.h"
#include "LvTheme.h"
#include "hal/Display.h"

// --- LvScreen base ---

void LvScreen::destroyUI() {
    // Content is owned by UIManager's _lvContent — just clear our pointers.
    // The UIManager calls lv_obj_clean(_lvContent) before creating the next screen.
    _screen = nullptr;
}

// --- UIManager ---

void UIManager::begin(LGFX_TDeck* gfx) {
    _gfx = gfx;
    _statusBar.setGfx(gfx);
    _tabBar.setGfx(gfx);

    // Initialize LVGL theme and create persistent UI structure
    lv_obj_t* scr = lv_scr_act();
    LvTheme::init(lv_disp_get_default());

    // Apply screen background style
    lv_obj_add_style(scr, LvTheme::styleScreen(), 0);

    // Create LVGL status bar (top)
    _lvStatusBar.create(scr);

    // Create LVGL tab bar (bottom)
    _lvTabBar.create(scr);

    // Create content area between status bar and tab bar
    _lvContent = lv_obj_create(scr);
    lv_obj_set_pos(_lvContent, 0, Theme::STATUS_BAR_H);
    lv_obj_set_size(_lvContent, Theme::CONTENT_W, Theme::CONTENT_H);
    lv_obj_set_style_bg_color(_lvContent, lv_color_hex(Theme::BG), 0);
    lv_obj_set_style_bg_opa(_lvContent, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(_lvContent, 0, 0);
    lv_obj_set_style_pad_all(_lvContent, 0, 0);
    lv_obj_set_style_radius(_lvContent, 0, 0);

    _lvglActive = true;

    Serial.println("[UI] LVGL UI structure created");
}

void UIManager::setScreen(Screen* screen) {
    // When setting a legacy screen, hide LVGL content and use legacy rendering
    if (_currentLvScreen) {
        _currentLvScreen->onExit();
        _currentLvScreen->destroyUI();
        _currentLvScreen = nullptr;
    }

    if (_currentScreen) {
        _currentScreen->onExit();
    }

    _currentScreen = screen;

    if (_currentScreen) {
        _currentScreen->onEnter();
        _currentScreen->markDirty();
    }

    // Hide LVGL layers for legacy rendering
    if (_lvglActive) {
        lv_obj_add_flag(_lvStatusBar.obj(), LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(_lvTabBar.obj(), LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(_lvContent, LV_OBJ_FLAG_HIDDEN);
    }

    forceRedraw();
}

void UIManager::setLvScreen(LvScreen* screen) {
    // Transition from legacy screen
    if (_currentScreen) {
        _currentScreen->onExit();
        _currentScreen = nullptr;
    }

    // Transition from previous LVGL screen
    if (_currentLvScreen) {
        _currentLvScreen->onExit();
        _currentLvScreen->destroyUI();
    }

    _currentLvScreen = screen;

    // Show LVGL layers
    if (_lvglActive) {
        if (!_bootMode) {
            lv_obj_clear_flag(_lvStatusBar.obj(), LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(_lvTabBar.obj(), LV_OBJ_FLAG_HIDDEN);
        }
        lv_obj_clear_flag(_lvContent, LV_OBJ_FLAG_HIDDEN);
    }

    if (_currentLvScreen) {
        // Clean content area
        lv_obj_clean(_lvContent);
        _currentLvScreen->createUI(_lvContent);
        _currentLvScreen->onEnter();
    }
}

void UIManager::setOverlay(Screen* overlay) {
    _overlay = overlay;
    if (_overlay) {
        _overlay->markDirty();
    }
    forceRedraw();
}

void UIManager::setBootMode(bool boot) {
    _bootMode = boot;
    if (_lvglActive) {
        if (boot) {
            lv_obj_add_flag(_lvStatusBar.obj(), LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(_lvTabBar.obj(), LV_OBJ_FLAG_HIDDEN);
            // In boot mode, content area is full screen
            lv_obj_set_pos(_lvContent, 0, 0);
            lv_obj_set_size(_lvContent, Theme::SCREEN_W, Theme::SCREEN_H);
        } else {
            lv_obj_clear_flag(_lvStatusBar.obj(), LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(_lvTabBar.obj(), LV_OBJ_FLAG_HIDDEN);
            lv_obj_set_pos(_lvContent, 0, Theme::STATUS_BAR_H);
            lv_obj_set_size(_lvContent, Theme::CONTENT_W, Theme::CONTENT_H);
        }
    }
    forceRedraw();
}

void UIManager::update() {
    _statusBar.update();
    _lvStatusBar.update();
    if (_currentScreen) _currentScreen->update();
    if (_currentLvScreen) _currentLvScreen->refreshUI();
}

void UIManager::render() {
    if (!_gfx) return;

    // If an LVGL screen is active, don't do legacy rendering
    if (_currentLvScreen) return;

    bool needStatusRedraw = _statusBar.isDirty();
    bool needTabRedraw = _tabBar.isDirty();
    bool needContentRedraw = (_currentScreen && _currentScreen->isDirty());
    bool needOverlayRedraw = (_overlay && _overlay->isDirty());

    if (!needStatusRedraw && !needTabRedraw && !needContentRedraw && !needOverlayRedraw)
        return;

    if (!_bootMode && needStatusRedraw) {
        _statusBar.draw(*_gfx);
        _statusBar.clearDirty();
    }

    if (needContentRedraw) {
        if (_bootMode) {
            _gfx->setClipRect(0, 0, Theme::SCREEN_W, Theme::SCREEN_H);
        } else {
            _gfx->setClipRect(0, Theme::CONTENT_Y, Theme::CONTENT_W, Theme::CONTENT_H);
        }
        _gfx->fillRect(0, _bootMode ? 0 : Theme::CONTENT_Y,
                        Theme::CONTENT_W, _bootMode ? Theme::SCREEN_H : Theme::CONTENT_H,
                        Theme::BG);
        _currentScreen->draw(*_gfx);
        _currentScreen->clearDirty();
        _gfx->clearClipRect();
    }

    if (needOverlayRedraw && _overlay) {
        _gfx->setClipRect(0, Theme::CONTENT_Y, Theme::CONTENT_W, Theme::CONTENT_H);
        _overlay->draw(*_gfx);
        _overlay->clearDirty();
        _gfx->clearClipRect();
    }

    if (!_bootMode && needTabRedraw) {
        _tabBar.draw(*_gfx);
        _tabBar.clearDirty();
    }
}

void UIManager::forceRedraw() {
    _statusBar.markDirty();
    _tabBar.markDirty();
    if (_currentScreen) _currentScreen->markDirty();
    if (_overlay) _overlay->markDirty();
}

bool UIManager::handleKey(const KeyEvent& event) {
    if (_overlay) {
        return _overlay->handleKey(event);
    }
    if (_currentLvScreen) {
        return _currentLvScreen->handleKey(event);
    }
    if (_currentScreen) {
        return _currentScreen->handleKey(event);
    }
    return false;
}

bool UIManager::handleLongPress() {
    if (_currentLvScreen) {
        return _currentLvScreen->handleLongPress();
    }
    return false;
}

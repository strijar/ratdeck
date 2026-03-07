#pragma once

#include "ui/UIManager.h"
#include <functional>

class ReticulumManager;
class SX1262;
class UserConfig;

class LvHomeScreen : public LvScreen {
public:
    void createUI(lv_obj_t* parent) override;
    void refreshUI() override;
    void onEnter() override;
    bool handleKey(const KeyEvent& event) override;

    void setReticulumManager(ReticulumManager* rns) { _rns = rns; }
    void setRadio(SX1262* radio) { _radio = radio; }
    void setUserConfig(UserConfig* cfg) { _cfg = cfg; }
    void setAnnounceCallback(std::function<void()> cb) { _announceCb = cb; }

    const char* title() const override { return "Home"; }

private:
    ReticulumManager* _rns = nullptr;
    SX1262* _radio = nullptr;
    UserConfig* _cfg = nullptr;
    std::function<void()> _announceCb;
    unsigned long _lastUptime = 0;
    uint32_t _lastHeap = 0;

    lv_obj_t* _lblId = nullptr;
    lv_obj_t* _lblTransport = nullptr;
    lv_obj_t* _lblPaths = nullptr;
    lv_obj_t* _lblLora = nullptr;
    lv_obj_t* _lblHeap = nullptr;
    lv_obj_t* _lblPsram = nullptr;
    lv_obj_t* _lblUptime = nullptr;
};

#pragma once

#include "ui/UIManager.h"
#include <functional>
#include <string>
#include <vector>

class LXMFManager;
class AnnounceManager;

class LvMessagesScreen : public LvScreen {
public:
    using OpenCallback = std::function<void(const std::string& peerHex)>;

    void createUI(lv_obj_t* parent) override;
    void refreshUI() override;
    void onEnter() override;
    bool handleKey(const KeyEvent& event) override;

    void setLXMFManager(LXMFManager* lxmf) { _lxmf = lxmf; }
    void setAnnounceManager(AnnounceManager* am) { _am = am; }
    void setOpenCallback(OpenCallback cb) { _onOpen = cb; }
    void setUIManager(class UIManager* ui) { _ui = ui; }
    bool handleLongPress() override;

    const char* title() const override { return "Messages"; }

private:
    void rebuildList();
    void updateSelection(int oldIdx, int newIdx);

    LXMFManager* _lxmf = nullptr;
    AnnounceManager* _am = nullptr;
    class UIManager* _ui = nullptr;
    OpenCallback _onOpen;
    int _lastConvCount = -1;
    int _selectedIdx = 0;
    bool _confirmDelete = false;

    lv_obj_t* _list = nullptr;
    lv_obj_t* _lblEmpty = nullptr;
    std::vector<lv_obj_t*> _rows;
};

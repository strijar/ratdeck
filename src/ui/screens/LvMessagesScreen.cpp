#include "LvMessagesScreen.h"
#include "ui/Theme.h"
#include "ui/UIManager.h"
#include "reticulum/LXMFManager.h"
#include "reticulum/AnnounceManager.h"
#include "storage/MessageStore.h"
#include <Arduino.h>

void LvMessagesScreen::createUI(lv_obj_t* parent) {
    _screen = parent;
    lv_obj_set_style_bg_color(parent, lv_color_hex(Theme::BG), 0);
    lv_obj_set_style_pad_all(parent, 0, 0);

    _lblEmpty = lv_label_create(parent);
    lv_obj_set_style_text_font(_lblEmpty, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(_lblEmpty, lv_color_hex(Theme::MUTED), 0);
    lv_label_set_text(_lblEmpty, "No conversations");
    lv_obj_center(_lblEmpty);

    _list = lv_obj_create(parent);
    lv_obj_set_size(_list, lv_pct(100), lv_pct(100));
    lv_obj_set_pos(_list, 0, 0);
    lv_obj_set_flex_grow(_list, 1);
    lv_obj_set_style_bg_color(_list, lv_color_hex(Theme::BG), 0);
    lv_obj_set_style_bg_opa(_list, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(_list, 0, 0);
    lv_obj_set_style_pad_all(_list, 0, 0);
    lv_obj_set_style_pad_row(_list, 0, 0);
    lv_obj_set_style_radius(_list, 0, 0);
    lv_obj_set_layout(_list, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(_list, LV_FLEX_FLOW_COLUMN);

    _lastConvCount = -1;
    rebuildList();
}

void LvMessagesScreen::onEnter() {
    _lastConvCount = -1;
    _selectedIdx = 0;
    rebuildList();
}

void LvMessagesScreen::refreshUI() {
    if (!_lxmf) return;
    int count = (int)_lxmf->conversations().size();
    if (count != _lastConvCount) {
        rebuildList();
    }
}

// Update only the selection highlight without rebuilding widgets
void LvMessagesScreen::updateSelection(int oldIdx, int newIdx) {
    if (oldIdx >= 0 && oldIdx < (int)_rows.size()) {
        lv_obj_set_style_bg_color(_rows[oldIdx], lv_color_hex(Theme::BG), 0);
    }
    if (newIdx >= 0 && newIdx < (int)_rows.size()) {
        lv_obj_set_style_bg_color(_rows[newIdx], lv_color_hex(Theme::SELECTION_BG), 0);
        lv_obj_scroll_to_view(_rows[newIdx], LV_ANIM_OFF);
    }
}

void LvMessagesScreen::rebuildList() {
    if (!_lxmf || !_list) return;
    int count = (int)_lxmf->conversations().size();
    _lastConvCount = count;
    _rows.clear();
    lv_obj_clean(_list);

    if (count == 0) {
        lv_obj_clear_flag(_lblEmpty, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(_list, LV_OBJ_FLAG_HIDDEN);
        return;
    }

    lv_obj_add_flag(_lblEmpty, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(_list, LV_OBJ_FLAG_HIDDEN);

    if (_selectedIdx >= count) _selectedIdx = count - 1;
    if (_selectedIdx < 0) _selectedIdx = 0;

    const auto& convs = _lxmf->conversations();
    const lv_font_t* font = &lv_font_montserrat_14;

    for (int i = 0; i < count; i++) {
        const auto& peerHex = convs[i];
        int unread = _lxmf->unreadCount(peerHex);

        lv_obj_t* row = lv_obj_create(_list);
        lv_obj_set_size(row, Theme::CONTENT_W, 28);
        lv_obj_set_style_bg_color(row, lv_color_hex(
            i == _selectedIdx ? Theme::SELECTION_BG : Theme::BG), 0);
        lv_obj_set_style_bg_opa(row, LV_OPA_COVER, 0);
        lv_obj_set_style_border_color(row, lv_color_hex(Theme::BORDER), 0);
        lv_obj_set_style_border_width(row, 1, 0);
        lv_obj_set_style_border_side(row, LV_BORDER_SIDE_BOTTOM, 0);
        lv_obj_set_style_pad_all(row, 0, 0);
        lv_obj_set_style_radius(row, 0, 0);
        lv_obj_clear_flag(row, LV_OBJ_FLAG_SCROLLABLE);

        // Peer name — check name cache (survives reboots)
        std::string displayName;
        if (_am) displayName = _am->lookupName(peerHex);
        if (displayName.empty()) displayName = peerHex.substr(0, 16);

        lv_obj_t* lbl = lv_label_create(row);
        lv_obj_set_style_text_font(lbl, font, 0);
        lv_obj_set_style_text_color(lbl, lv_color_hex(Theme::PRIMARY), 0);
        lv_label_set_text(lbl, displayName.c_str());
        lv_obj_align(lbl, LV_ALIGN_LEFT_MID, 4, 0);

        // Unread badge
        if (unread > 0) {
            char badge[8];
            snprintf(badge, sizeof(badge), "(%d)", unread);
            lv_obj_t* badgeLbl = lv_label_create(row);
            lv_obj_set_style_text_font(badgeLbl, font, 0);
            lv_obj_set_style_text_color(badgeLbl, lv_color_hex(Theme::BADGE_BG), 0);
            lv_label_set_text(badgeLbl, badge);
            lv_obj_align(badgeLbl, LV_ALIGN_RIGHT_MID, -4, 0);
        }

        _rows.push_back(row);
    }
}

bool LvMessagesScreen::handleLongPress() {
    if (!_lxmf) return false;
    int count = (int)_lxmf->conversations().size();
    if (count == 0 || _selectedIdx >= count) return false;
    _confirmDelete = true;
    if (_ui) _ui->lvStatusBar().showToast("Delete chat? Enter=Yes Esc=No", 5000);
    return true;
}

bool LvMessagesScreen::handleKey(const KeyEvent& event) {
    if (!_lxmf) return false;

    // Confirm delete mode
    if (_confirmDelete) {
        if (event.enter || event.character == '\n' || event.character == '\r') {
            int count = (int)_lxmf->conversations().size();
            if (_selectedIdx < count) {
                const auto& peerHex = _lxmf->conversations()[_selectedIdx];
                _lxmf->markRead(peerHex);  // Clear unread
                // Delete via MessageStore
                extern MessageStore messageStore;
                messageStore.deleteConversation(peerHex);
                messageStore.refreshConversations();
                if (_ui) _ui->lvStatusBar().showToast("Chat deleted", 1200);
                _selectedIdx = 0;
                _lastConvCount = -1;
                rebuildList();
            }
            _confirmDelete = false;
            return true;
        }
        _confirmDelete = false;
        if (_ui) _ui->lvStatusBar().showToast("Cancelled", 800);
        return true;
    }

    int count = (int)_lxmf->conversations().size();
    if (count == 0) return false;

    if (event.up) {
        if (_selectedIdx > 0) {
            int prev = _selectedIdx;
            _selectedIdx--;
            updateSelection(prev, _selectedIdx);
        }
        return true;
    }
    if (event.down) {
        if (_selectedIdx < count - 1) {
            int prev = _selectedIdx;
            _selectedIdx++;
            updateSelection(prev, _selectedIdx);
        }
        return true;
    }
    if (event.enter || event.character == '\n' || event.character == '\r') {
        if (_selectedIdx < count && _onOpen) {
            _onOpen(_lxmf->conversations()[_selectedIdx]);
        }
        return true;
    }
    return false;
}

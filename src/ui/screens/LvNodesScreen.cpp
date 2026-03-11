#include "LvNodesScreen.h"
#include "ui/Theme.h"
#include "ui/LvTheme.h"
#include "ui/UIManager.h"
#include "reticulum/AnnounceManager.h"
#include <Arduino.h>
#include <algorithm>

void LvNodesScreen::createUI(lv_obj_t* parent) {
    _screen = parent;
    lv_obj_set_style_bg_color(parent, lv_color_hex(Theme::BG), 0);
    lv_obj_set_style_pad_all(parent, 0, 0);

    // Empty state label
    _lblEmpty = lv_label_create(parent);
    lv_obj_set_style_text_font(_lblEmpty, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(_lblEmpty, lv_color_hex(Theme::MUTED), 0);
    lv_label_set_text(_lblEmpty, "No nodes discovered");
    lv_obj_center(_lblEmpty);

    // Scrollable list container
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

    // Pre-allocate ROW_POOL_SIZE row widgets
    const lv_font_t* font = &lv_font_montserrat_14;
    const lv_font_t* smallFont = &lv_font_montserrat_10;

    for (int i = 0; i < ROW_POOL_SIZE; i++) {
        lv_obj_t* row = lv_obj_create(_list);
        lv_obj_set_size(row, Theme::CONTENT_W, 24);
        lv_obj_set_style_bg_color(row, lv_color_hex(Theme::BG), 0);
        lv_obj_set_style_bg_opa(row, LV_OPA_COVER, 0);
        lv_obj_set_style_border_width(row, 0, 0);
        lv_obj_set_style_pad_all(row, 0, 0);
        lv_obj_set_style_radius(row, 0, 0);
        lv_obj_clear_flag(row, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_add_flag(row, LV_OBJ_FLAG_HIDDEN);

        lv_obj_t* nameLbl = lv_label_create(row);
        lv_obj_set_style_text_font(nameLbl, font, 0);
        lv_obj_set_style_text_color(nameLbl, lv_color_hex(Theme::PRIMARY), 0);
        lv_label_set_text(nameLbl, "");
        lv_obj_align(nameLbl, LV_ALIGN_LEFT_MID, 8, 0);

        lv_obj_t* infoLbl = lv_label_create(row);
        lv_obj_set_style_text_font(infoLbl, smallFont, 0);
        lv_obj_set_style_text_color(infoLbl, lv_color_hex(Theme::SECONDARY), 0);
        lv_label_set_text(infoLbl, "");
        lv_obj_align(infoLbl, LV_ALIGN_RIGHT_MID, -4, 0);

        _poolRows[i] = row;
        _poolNameLabels[i] = nameLbl;
        _poolInfoLabels[i] = infoLbl;
    }

    _lastNodeCount = -1;
    _lastContactCount = -1;
    updateSortOrder();
    syncVisibleRows();
}

void LvNodesScreen::onEnter() {
    _lastNodeCount = -1;
    _lastContactCount = -1;
    _selectedIdx = 0;
    _viewportStart = 0;
    updateSortOrder();
    syncVisibleRows();
}

void LvNodesScreen::refreshUI() {
    if (!_am) return;
    unsigned long now = millis();
    if (now - _lastRebuild < REBUILD_INTERVAL_MS) return;
    int contacts = 0;
    for (const auto& n : _am->nodes()) { if (n.saved) contacts++; }
    int countDelta = abs(_am->nodeCount() - _lastNodeCount);
    int contactDelta = abs(contacts - _lastContactCount);
    if (countDelta > 0 || contactDelta > 0) {
        _lastRebuild = now;
        if (countDelta > 3 || contactDelta > 0) {
            updateSortOrder();
            syncVisibleRows();
        }
    }
}

void LvNodesScreen::updateSortOrder() {
    if (!_am) return;
    const auto& nodes = _am->nodes();
    int count = (int)nodes.size();
    _lastNodeCount = count;

    _sortedContactIndices.clear();
    _sortedOnlineIndices.clear();

    for (int i = 0; i < count; i++) {
        if (nodes[i].saved) _sortedContactIndices.push_back(i);
        else _sortedOnlineIndices.push_back(i);
    }
    _lastContactCount = (int)_sortedContactIndices.size();

    // Sort online nodes: most recently seen first
    std::sort(_sortedOnlineIndices.begin(), _sortedOnlineIndices.end(), [&nodes](int a, int b) {
        return nodes[a].lastSeen > nodes[b].lastSeen;
    });

    // Total entries: contacts header (if any) + contacts + online header + online nodes
    _totalEntries = 0;
    if (!_sortedContactIndices.empty()) _totalEntries += 1 + (int)_sortedContactIndices.size();
    if (count > 0) _totalEntries += 1 + (int)_sortedOnlineIndices.size();

    // Clamp selection
    if (_selectedIdx >= _totalEntries) _selectedIdx = _totalEntries - 1;
    if (_selectedIdx < 0) _selectedIdx = 0;
    // Skip headers
    while (_selectedIdx < _totalEntries && getNodeIdxForEntry(_selectedIdx) == -1) _selectedIdx++;
    if (_selectedIdx >= _totalEntries) _selectedIdx = _totalEntries - 1;

    _dataChanged = true;
}

// Map a logical entry index to a node index, or -1 for section headers
static int mapEntryToNodeIdx(int entry, const std::vector<int>& contacts, const std::vector<int>& online) {
    int pos = 0;
    if (!contacts.empty()) {
        if (entry == pos) return -1; // Contacts header
        pos++;
        if (entry < pos + (int)contacts.size()) return contacts[entry - pos];
        pos += (int)contacts.size();
    }
    // Online header
    if (entry == pos) return -1;
    pos++;
    if (entry < pos + (int)online.size()) return online[entry - pos];
    return -1;
}

// Determine if entry is a header, and which section
// Returns: 0 = contacts header, 1 = online header, -1 = not a header
static int headerType(int entry, const std::vector<int>& contacts, const std::vector<int>& online) {
    int pos = 0;
    if (!contacts.empty()) {
        if (entry == pos) return 0;
        pos += 1 + (int)contacts.size();
    }
    if (entry == pos) return 1;
    return -1;
}

void LvNodesScreen::syncVisibleRows() {
    if (!_am || !_list) return;

    if (_totalEntries == 0) {
        lv_obj_clear_flag(_lblEmpty, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(_list, LV_OBJ_FLAG_HIDDEN);
        for (int i = 0; i < ROW_POOL_SIZE; i++) lv_obj_add_flag(_poolRows[i], LV_OBJ_FLAG_HIDDEN);
        return;
    }

    lv_obj_add_flag(_lblEmpty, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(_list, LV_OBJ_FLAG_HIDDEN);

    const auto& nodes = _am->nodes();

    // Compute viewport: center selected item with buffer
    int halfPool = ROW_POOL_SIZE / 2;
    _viewportStart = _selectedIdx - halfPool;
    if (_viewportStart < 0) _viewportStart = 0;
    if (_viewportStart + ROW_POOL_SIZE > _totalEntries) {
        _viewportStart = _totalEntries - ROW_POOL_SIZE;
        if (_viewportStart < 0) _viewportStart = 0;
    }

    for (int i = 0; i < ROW_POOL_SIZE; i++) {
        int entryIdx = _viewportStart + i;
        if (entryIdx >= _totalEntries) {
            lv_obj_add_flag(_poolRows[i], LV_OBJ_FLAG_HIDDEN);
            continue;
        }

        lv_obj_clear_flag(_poolRows[i], LV_OBJ_FLAG_HIDDEN);
        bool isSelected = (entryIdx == _selectedIdx);

        int hdr = headerType(entryIdx, _sortedContactIndices, _sortedOnlineIndices);
        if (hdr >= 0) {
            // Section header row
            char buf[32];
            if (hdr == 0) {
                snprintf(buf, sizeof(buf), "Contacts (%d)", (int)_sortedContactIndices.size());
            } else {
                snprintf(buf, sizeof(buf), "Online (%d)", (int)_sortedOnlineIndices.size());
            }
            lv_obj_set_size(_poolRows[i], Theme::CONTENT_W, 22);
            lv_obj_set_style_bg_color(_poolRows[i], lv_color_hex(Theme::BG), 0);
            lv_obj_set_style_border_width(_poolRows[i], 1, 0);
            lv_obj_set_style_border_color(_poolRows[i], lv_color_hex(Theme::BORDER), 0);
            lv_obj_set_style_border_side(_poolRows[i], LV_BORDER_SIDE_BOTTOM, 0);
            lv_obj_set_style_text_font(_poolNameLabels[i], &lv_font_montserrat_12, 0);
            lv_obj_set_style_text_color(_poolNameLabels[i], lv_color_hex(Theme::ACCENT), 0);
            lv_label_set_text(_poolNameLabels[i], buf);
            lv_obj_align(_poolNameLabels[i], LV_ALIGN_LEFT_MID, 4, 0);
            lv_label_set_text(_poolInfoLabels[i], "");
        } else {
            // Node row
            int nodeIdx = mapEntryToNodeIdx(entryIdx, _sortedContactIndices, _sortedOnlineIndices);
            if (nodeIdx < 0 || nodeIdx >= (int)nodes.size()) {
                lv_obj_add_flag(_poolRows[i], LV_OBJ_FLAG_HIDDEN);
                continue;
            }
            const auto& node = nodes[nodeIdx];

            lv_obj_set_size(_poolRows[i], Theme::CONTENT_W, 24);
            lv_obj_set_style_bg_color(_poolRows[i], lv_color_hex(
                isSelected ? Theme::SELECTION_BG : Theme::BG), 0);
            lv_obj_set_style_border_width(_poolRows[i], 0, 0);
            lv_obj_set_style_border_side(_poolRows[i], LV_BORDER_SIDE_NONE, 0);

            // Name + hash
            std::string truncName = node.name.substr(0, 15);
            std::string displayHash = node.hash.toHex().substr(0, 12);
            char buf[64];
            snprintf(buf, sizeof(buf), "%s [%s]", truncName.c_str(), displayHash.c_str());
            lv_obj_set_style_text_font(_poolNameLabels[i], &lv_font_montserrat_14, 0);
            lv_obj_set_style_text_color(_poolNameLabels[i], lv_color_hex(
                node.saved ? Theme::ACCENT : Theme::PRIMARY), 0);
            lv_label_set_text(_poolNameLabels[i], buf);
            lv_obj_align(_poolNameLabels[i], LV_ALIGN_LEFT_MID, 8, 0);

            // Hops + age
            unsigned long ageSec = (millis() - node.lastSeen) / 1000;
            char infoBuf[24];
            if (node.hops < 128) {
                if (ageSec < 60) snprintf(infoBuf, sizeof(infoBuf), "%dhop %lus", node.hops, ageSec);
                else snprintf(infoBuf, sizeof(infoBuf), "%dhop %lum", node.hops, ageSec / 60);
            } else {
                if (ageSec < 60) snprintf(infoBuf, sizeof(infoBuf), "%lus", ageSec);
                else snprintf(infoBuf, sizeof(infoBuf), "%lum", ageSec / 60);
            }
            lv_label_set_text(_poolInfoLabels[i], infoBuf);
            lv_obj_align(_poolInfoLabels[i], LV_ALIGN_RIGHT_MID, -4, 0);
        }
    }

    _dataChanged = false;
}

// Helper: get node index for a given logical entry, -1 for headers
int LvNodesScreen::getNodeIdxForEntry(int entry) const {
    return mapEntryToNodeIdx(entry, _sortedContactIndices, _sortedOnlineIndices);
}

void LvNodesScreen::scrollToSelected() {
    // With widget pool, scrolling is handled by viewport recomputation in syncVisibleRows
    // Find which pool row corresponds to selected entry and scroll to it
    int poolIdx = _selectedIdx - _viewportStart;
    if (poolIdx >= 0 && poolIdx < ROW_POOL_SIZE && _poolRows[poolIdx]) {
        lv_obj_scroll_to_view(_poolRows[poolIdx], LV_ANIM_OFF);
    }
}

bool LvNodesScreen::handleLongPress() {
    if (!_am || _totalEntries == 0) return false;
    if (_selectedIdx < 0 || _selectedIdx >= _totalEntries) return false;
    int nodeIdx = getNodeIdxForEntry(_selectedIdx);
    if (nodeIdx < 0 || nodeIdx >= (int)_am->nodes().size()) return false;
    const auto& node = _am->nodes()[nodeIdx];
    if (node.saved) {
        _confirmDelete = true;
        if (_ui) _ui->lvStatusBar().showToast("Remove friend? Enter=Yes Esc=No", 5000);
    } else {
        auto& mutableNode = const_cast<DiscoveredNode&>(node);
        mutableNode.saved = true;
        _am->saveContacts();
        if (_ui) _ui->lvStatusBar().showToast("Added to friends!", 1200);
        updateSortOrder();
        syncVisibleRows();
    }
    return true;
}

bool LvNodesScreen::handleKey(const KeyEvent& event) {
    if (!_am || _totalEntries == 0) return false;

    // Confirm delete mode
    if (_confirmDelete) {
        if (event.enter || event.character == '\n' || event.character == '\r') {
            int nodeIdx = getNodeIdxForEntry(_selectedIdx);
            if (nodeIdx >= 0 && nodeIdx < (int)_am->nodes().size()) {
                auto& nodes = const_cast<std::vector<DiscoveredNode>&>(_am->nodes());
                nodes.erase(nodes.begin() + nodeIdx);
                _am->saveContacts();
                if (_ui) _ui->lvStatusBar().showToast("Contact deleted", 1200);
                _selectedIdx = 0;
                updateSortOrder();
                syncVisibleRows();
            }
            _confirmDelete = false;
            return true;
        }
        _confirmDelete = false;
        if (_ui) _ui->lvStatusBar().showToast("Cancelled", 800);
        return true;
    }

    if (event.up) {
        int prev = _selectedIdx;
        _selectedIdx--;
        // Skip headers
        while (_selectedIdx >= 0 && getNodeIdxForEntry(_selectedIdx) == -1) _selectedIdx--;
        if (_selectedIdx < 0) _selectedIdx = prev;
        if (_selectedIdx != prev) syncVisibleRows();
        return true;
    }
    if (event.down) {
        int prev = _selectedIdx;
        _selectedIdx++;
        while (_selectedIdx < _totalEntries && getNodeIdxForEntry(_selectedIdx) == -1) _selectedIdx++;
        if (_selectedIdx >= _totalEntries) _selectedIdx = prev;
        if (_selectedIdx != prev) syncVisibleRows();
        return true;
    }
    if (event.enter || event.character == '\n' || event.character == '\r') {
        int nodeIdx = getNodeIdxForEntry(_selectedIdx);
        if (nodeIdx >= 0 && nodeIdx < (int)_am->nodes().size() && _onSelect) {
            _onSelect(_am->nodes()[nodeIdx].hash.toHex());
        }
        return true;
    }
    // 's' or 'S' to save/unsave contact
    if (event.character == 's' || event.character == 'S') {
        int nodeIdx = getNodeIdxForEntry(_selectedIdx);
        if (nodeIdx >= 0 && nodeIdx < (int)_am->nodes().size()) {
            auto& node = const_cast<DiscoveredNode&>(_am->nodes()[nodeIdx]);
            node.saved = !node.saved;
            if (node.saved) {
                _am->saveContacts();
            }
            updateSortOrder();
            syncVisibleRows();
        }
        return true;
    }
    return false;
}

#ifdef AURA_MAC

#include "controls/controls_extra_mac.h"
#include <algorithm>

namespace aura {
namespace mac {

// ============================================================================
// ListBoxMac
// ============================================================================

ListBoxMac::ListBoxMac() : table_(nil), scroll_(nil), selected_index_(-1) {}
ListBoxMac::~ListBoxMac() {
    if (table_) [table_ release];
    if (scroll_) [scroll_ release];
}

void ListBoxMac::AddItem(const std::wstring& item) {
    items_.push_back(item);
    if (table_) [table_ reloadData];
}

void ListBoxMac::RemoveItem(int index) {
    if (index >= 0 && index < (int)items_.size()) {
        items_.erase(items_.begin() + index);
        if (table_) [table_ reloadData];
    }
}

void ListBoxMac::Clear() {
    items_.clear();
    if (table_) [table_ reloadData];
}

void ListBoxMac::SetSelectedIndex(int index) {
    selected_index_ = index;
    if (table_ && index >= 0) {
        [table_ selectRowIndexes:[NSIndexSet indexSetWithIndex:index]
             byExtendingSelection:NO];
    }
}

int ListBoxMac::GetSelectedIndex() const {
    return selected_index_;
}

std::wstring ListBoxMac::GetItem(int index) const {
    if (index >= 0 && index < (int)items_.size()) return items_[index];
    return L"";
}

void ListBoxMac::SetBounds(int x, int y, int width, int height) {
    ViewMac::SetBounds(x, y, width, height);
    if (scroll_) [scroll_ setFrame:NSMakeRect(x, y, width, height)];
}

// ============================================================================
// ScrollBarMac
// ============================================================================

ScrollBarMac::ScrollBarMac() : slider_(nil), min_(0), max_(100), value_(0), page_size_(10) {}
ScrollBarMac::~ScrollBarMac() {
    if (slider_) [slider_ release];
}

void ScrollBarMac::SetValue(int value) {
    value_ = std::max(min_, std::min(max_, value));
    if (slider_) [slider_ setDoubleValue:value_];
}

void ScrollBarMac::SetRange(int min, int max) {
    min_ = min;
    max_ = max;
    if (slider_) {
        [slider_ setMinValue:min];
        [slider_ setMaxValue:max];
    }
}

void ScrollBarMac::SetPageSize(int pageSize) {
    page_size_ = pageSize;
}

void ScrollBarMac::SetBounds(int x, int y, int width, int height) {
    ViewMac::SetBounds(x, y, width, height);
    if (slider_) [slider_ setFrame:NSMakeRect(x, y, width, height)];
}

// ============================================================================
// TabControlMac
// ============================================================================

TabControlMac::TabControlMac() : tabview_(nil), selected_(0) {}
TabControlMac::~TabControlMac() {
    if (tabview_) [tabview_ release];
}

void TabControlMac::AddTab(const std::wstring& title) {
    tabs_.push_back(title);
    if (tabview_) {
        NSTabViewItem* item = [[NSTabViewItem alloc] init];
        NSString* s = [NSString stringWithUTF8String:std::string(title.begin(), title.end()).c_str()];
        [item setLabel:s];
        [tabview_ addTabViewItem:item];
    }
}

void TabControlMac::RemoveTab(int index) {
    if (index >= 0 && index < (int)tabs_.size()) {
        tabs_.erase(tabs_.begin() + index);
    }
}

void TabControlMac::Clear() {
    tabs_.clear();
    if (tabview_) {
        while ([[tabview_ tabViewItems] count] > 0) {
            [tabview_ removeTabViewItem:[[tabview_ tabViewItems] objectAtIndex:0]];
        }
    }
}

void TabControlMac::SetSelectedTab(int index) {
    selected_ = index;
    if (tabview_ && index >= 0 && index < (int)tabs_.size()) {
        [tabview_ selectTabViewItemAtIndex:index];
    }
}

} // namespace mac
} // namespace aura

#endif // AURA_MAC

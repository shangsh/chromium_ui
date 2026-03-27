#ifdef AURA_MAC

#include "controls/controls_extra_mac.h"

namespace aura {
namespace mac {

// ComboBox implementation
ComboBoxMac::ComboBoxMac() : combobox_(nil), selected_index_(-1) {
}

ComboBoxMac::~ComboBoxMac() {
    if (combobox_) {
        [combobox_ release];
    }
}

void ComboBoxMac::AddItem(const std::wstring& item) {
    items_.push_back(item);
    if (combobox_) {
        NSString* s = [NSString stringWithUTF8String:std::string(item.begin(), item.end()).c_str()];
        [combobox_ addItemWithTitle:s];
    }
}

void ComboBoxMac::RemoveItem(int index) {
    if (index >= 0 && index < (int)items_.size()) {
        items_.erase(items_.begin() + index);
        if (combobox_) {
            [combobox_ removeItemAtIndex:index];
        }
    }
}

void ComboBoxMac::Clear() {
    items_.clear();
    if (combobox_) {
        [combobox_ removeAllItems];
    }
}

void ComboBoxMac::SetSelectedIndex(int index) {
    selected_index_ = index;
    if (combobox_ && index >= 0 && index < (int)items_.size()) {
        [combobox_ selectItemAtIndex:index];
    }
}

int ComboBoxMac::GetSelectedIndex() const {
    return selected_index_;
}

std::wstring ComboBoxMac::GetSelectedItem() const {
    if (selected_index_ >= 0 && selected_index_ < (int)items_.size()) {
        return items_[selected_index_];
    }
    return L"";
}

void ComboBoxMac::SetBounds(int x, int y, int width, int height) {
    ViewMac::SetBounds(x, y, width, height);
    if (combobox_) {
        [combobox_ setFrame:NSMakeRect(x, y, width, height)];
    }
}

// ListBox implementation
ListBoxMac::ListBoxMac() : table_(nil), scroll_(nil), selected_index_(-1) {
}

ListBoxMac::~ListBoxMac() {
    if (table_) [table_ release];
    if (scroll_) [scroll_ release];
}

void ListBoxMac::AddItem(const std::wstring& item) {
    items_.push_back(item);
}

void ListBoxMac::RemoveItem(int index) {
    if (index >= 0 && index < (int)items_.size()) {
        items_.erase(items_.begin() + index);
    }
}

void ListBoxMac::Clear() {
    items_.clear();
}

void ListBoxMac::SetSelectedIndex(int index) {
    selected_index_ = index;
}

int ListBoxMac::GetSelectedIndex() const {
    return selected_index_;
}

std::wstring ListBoxMac::GetItem(int index) const {
    if (index >= 0 && index < (int)items_.size()) {
        return items_[index];
    }
    return L"";
}

void ListBoxMac::SetBounds(int x, int y, int width, int height) {
    ViewMac::SetBounds(x, y, width, height);
    if (scroll_) {
        [scroll_ setFrame:NSMakeRect(x, y, width, height)];
    }
}

// ScrollBar implementation
ScrollBarMac::ScrollBarMac() : slider_(nil), min_(0), max_(100), value_(0), page_size_(10) {
}

ScrollBarMac::~ScrollBarMac() {
    if (slider_) [slider_ release];
}

void ScrollBarMac::SetValue(int value) {
    value_ = std::max(min_, std::min(max_, value));
    if (slider_) {
        [slider_ setDoubleValue:value_];
    }
}

int ScrollBarMac::GetValue() const {
    return value_;
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
    if (slider_) {
        [slider_ setFrame:NSMakeRect(x, y, width, height)];
    }
}

// ProgressBar implementation
ProgressBarMac::ProgressBarMac() : progress_(nil), min_(0), max_(100), value_(0), indeterminate_(false) {
}

ProgressBarMac::~ProgressBarMac() {
    if (progress_) [progress_ release];
}

void ProgressBarMac::SetValue(int value) {
    value_ = std::max(min_, std::min(max_, value));
    if (progress_ && !indeterminate_) {
        [progress_ setDoubleValue:value_];
    }
}

int ProgressBarMac::GetValue() const {
    return value_;
}

void ProgressBarMac::SetRange(int min, int max) {
    min_ = min;
    max_ = max;
    if (progress_) {
        [progress_ setMinValue:min];
        [progress_ setMaxValue:max];
    }
}

void ProgressBarMac::SetIndeterminate(bool indeterminate) {
    indeterminate_ = indeterminate;
    if (progress_) {
        if (indeterminate) {
            [progress_ setIndeterminate:YES];
            [progress_ startAnimation:nil];
        } else {
            [progress_ setIndeterminate:NO];
            [progress_ stopAnimation:nil];
        }
    }
}

void ProgressBarMac::SetBounds(int x, int y, int width, int height) {
    ViewMac::SetBounds(x, y, width, height);
    if (progress_) {
        [progress_ setFrame:NSMakeRect(x, y, width, height)];
    }
}

// TabControl implementation
TabControlMac::TabControlMac() : tabview_(nil), selected_(0) {
}

TabControlMac::~TabControlMac() {
    if (tabview_) [tabview_ release];
}

void TabControlMac::AddTab(const std::wstring& title) {
    tabs_.push_back(title);
    if (tabview_) {
        NSTabViewItem* item = [[[NSTabViewItem alloc] init] autorelease];
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
        // NSTabView has no removeAllItems; remove each tab individually
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

int TabControlMac::GetSelectedTab() const {
    return selected_;
}

// Slider implementation
SliderMac::SliderMac() : slider_(nil), min_(0), max_(100), value_(50) {
}

SliderMac::~SliderMac() {
    if (slider_) [slider_ release];
}

void SliderMac::SetValue(int value) {
    value_ = std::max(min_, std::min(max_, value));
    if (slider_) {
        [slider_ setDoubleValue:value_];
    }
}

int SliderMac::GetValue() const {
    return value_;
}

void SliderMac::SetRange(int min, int max) {
    min_ = min;
    max_ = max;
    if (slider_) {
        [slider_ setMinValue:min];
        [slider_ setMaxValue:max];
    }
}

void SliderMac::SetTickCount(int count) {
    // macOS slider tick marks
}

void SliderMac::SetBounds(int x, int y, int width, int height) {
    ViewMac::SetBounds(x, y, width, height);
    if (slider_) {
        [slider_ setFrame:NSMakeRect(x, y, width, height)];
    }
}

} // namespace mac
} // namespace aura

#endif // AURA_MAC

#ifndef AURA_MAC_CONTROLS_EXTRA_H_
#define AURA_MAC_CONTROLS_EXTRA_H_

// Additional macOS-specific Aura Controls
// This file only compiled on macOS with -DAURA_MAC flag

#ifdef AURA_MAC

#import <Cocoa/Cocoa.h>

#include <string>
#include <vector>
#include "controls/controls_mac.h"

namespace aura {
namespace mac {

// ComboBox - Dropdown
class ComboBoxMac : public ViewMac {
public:
    ComboBoxMac();
    virtual ~ComboBoxMac();
    
    void AddItem(const std::wstring& item);
    void RemoveItem(int index);
    void Clear();
    
    void SetSelectedIndex(int index);
    int GetSelectedIndex() const;
    std::wstring GetSelectedItem() const;
    
    NSPopUpButton* GetNSComboBox() const { return combobox_; }
    
    virtual void SetBounds(int x, int y, int width, int height) override;
    
private:
    NSPopUpButton* combobox_;
    std::vector<std::wstring> items_;
    int selected_index_;
};

// ListBox
class ListBoxMac : public ViewMac {
public:
    ListBoxMac();
    virtual ~ListBoxMac();
    
    void AddItem(const std::wstring& item);
    void RemoveItem(int index);
    void Clear();
    
    void SetSelectedIndex(int index);
    int GetSelectedIndex() const;
    
    int GetItemCount() const { return (int)items_.size(); }
    std::wstring GetItem(int index) const;
    
    NSTableView* GetNSListBox() const { return table_; }
    
    virtual void SetBounds(int x, int y, int width, int height) override;
    
private:
    NSTableView* table_;
    NSScrollView* scroll_;
    std::vector<std::wstring> items_;
    int selected_index_;
};

// ScrollBar
class ScrollBarMac : public ViewMac {
public:
    ScrollBarMac();
    virtual ~ScrollBarMac();
    
    void SetValue(int value);
    int GetValue() const;
    
    void SetRange(int min, int max);
    void SetPageSize(int pageSize);
    
    NSSlider* GetNSSlider() const { return slider_; }
    
    virtual void SetBounds(int x, int y, int width, int height) override;
    
private:
    NSSlider* slider_;
    int min_;
    int max_;
    int value_;
    int page_size_;
};

// ProgressBar
class ProgressBarMac : public ViewMac {
public:
    ProgressBarMac();
    virtual ~ProgressBarMac();
    
    void SetValue(int value);
    int GetValue() const;
    
    void SetRange(int min, int max);
    
    void SetIndeterminate(bool indeterminate);
    bool IsIndeterminate() const { return indeterminate_; }
    
    NSProgressIndicator* GetNSProgressBar() const { return progress_; }
    
    virtual void SetBounds(int x, int y, int width, int height) override;
    
private:
    NSProgressIndicator* progress_;
    int min_;
    int max_;
    int value_;
    bool indeterminate_;
};

// TabControl
class TabControlMac : public ViewMac {
public:
    TabControlMac();
    virtual ~TabControlMac();
    
    void AddTab(const std::wstring& title);
    void RemoveTab(int index);
    void Clear();
    
    void SetSelectedTab(int index);
    int GetSelectedTab() const;
    
    NSTabView* GetNSTabControl() const { return tabview_; }
    
private:
    NSTabView* tabview_;
    std::vector<std::wstring> tabs_;
    int selected_;
};

// Slider
class SliderMac : public ViewMac {
public:
    SliderMac();
    virtual ~SliderMac();
    
    void SetValue(int value);
    int GetValue() const;
    
    void SetRange(int min, int max);
    void SetTickCount(int count);
    
    NSSlider* GetNSSlider() const { return slider_; }
    
    virtual void SetBounds(int x, int y, int width, int height) override;
    
private:
    NSSlider* slider_;
    int min_;
    int max_;
    int value_;
};

} // namespace mac
} // namespace aura

#endif // AURA_MAC
#endif // AURA_MAC_CONTROLS_EXTRA_H_

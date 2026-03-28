#ifndef AURA_MAC_CONTROLS_H_
#define AURA_MAC_CONTROLS_H_

// macOS-specific Aura Controls implementation
// This file only compiled on macOS with -DAURA_MAC flag

#ifdef AURA_MAC

#import <Cocoa/Cocoa.h>

#include <string>
#include <vector>
#include <functional>
#include "../view/view_mac.h"

// Forward declare ObjC classes at global scope (ObjC++ requirement)
@class AuraButtonTarget;
@class AuraTextFieldTarget;
@class AuraSliderTarget;
@class AuraCheckboxTarget;
@class AuraComboBoxTarget;
@class AuraListViewDataSource;

namespace aura {
namespace mac {

// ============================================================================
// ControlMac - base class for all aura controls
// ============================================================================

class ControlMac {
public:
    virtual ~ControlMac() {}
    virtual void SetBounds(int x, int y, int width, int height) = 0;
    virtual void SetVisible(bool visible);
    virtual void SetEnabled(bool enabled);
    NSView* GetNSView() const { return nsview_; }
protected:
    NSView* nsview_ = nil;
};

// ============================================================================
// ButtonMac
// ============================================================================

class ButtonMac : public ControlMac {
public:
    ButtonMac();
    virtual ~ButtonMac();
    
    void Init(int x, int y, int width, int height);
    
    void SetText(const std::wstring& text);
    std::wstring GetText() const { return text_; }
    
    void SetOnClick(std::function<void()> fn);
    
    NSButton* GetNSButton() const { return button_; }
    
    virtual void SetBounds(int x, int y, int width, int height) override;
    
private:
    NSButton* button_ = nil;
    std::wstring text_;
    std::function<void()> onClick_;
    id target_ = nil;
};

// ============================================================================
// LabelMac
// ============================================================================

class LabelMac : public ControlMac {
public:
    LabelMac();
    LabelMac(const std::wstring& text);
    virtual ~LabelMac();
    
    void Init(int x, int y, int width, int height);
    void Init(int x, int y, int width, int height, const std::wstring& text);
    
    void SetText(const std::wstring& text);
    std::wstring GetText() const { return text_; }
    
    void SetFont(NSFont* font);
    void SetTextColor(NSColor* color);
    void SetAlignment(NSTextAlignment align);
    
    virtual void SetBounds(int x, int y, int width, int height) override;
    
private:
    NSTextField* label_ = nil;
    std::wstring text_;
};

// ============================================================================
// TextFieldMac
// ============================================================================

class TextFieldMac : public ControlMac {
public:
    TextFieldMac();
    virtual ~TextFieldMac();
    
    void Init(int x, int y, int width, int height);
    
    void SetText(const std::wstring& text);
    std::wstring GetText() const;
    
    void SetPlaceholder(const std::wstring& text);
    
    void SetPasswordMode(bool password);
    bool IsPasswordMode() const { return password_; }
    
    void SetReadOnly(bool read_only);
    bool IsReadOnly() const { return read_only_; }
    
    void SetOnChange(std::function<void()> fn);
    
    virtual void SetBounds(int x, int y, int width, int height) override;
    
private:
    NSTextField* textfield_ = nil;
    std::wstring placeholder_;
    bool password_ = false;
    bool read_only_ = false;
    std::function<void()> onChange_;
    id target_ = nil;
};

// ============================================================================
// CheckboxMac
// ============================================================================

class CheckboxMac : public ControlMac {
public:
    CheckboxMac();
    CheckboxMac(const std::wstring& text);
    virtual ~CheckboxMac();
    
    void Init(int x, int y, int width, int height);
    void Init(int x, int y, int width, int height, const std::wstring& text);
    
    void SetText(const std::wstring& text);
    std::wstring GetText() const { return text_; }
    
    void SetChecked(bool checked);
    bool IsChecked() const;
    
    void SetOnChange(std::function<void()> fn);
    
    virtual void SetBounds(int x, int y, int width, int height) override;
    
private:
    NSButton* checkbox_ = nil;
    std::wstring text_;
    std::function<void()> onChange_;
    id target_ = nil;
};

// ============================================================================
// SliderMac
// ============================================================================

class SliderMac : public ControlMac {
public:
    SliderMac();
    virtual ~SliderMac();
    
    void Init(int x, int y, int width, int height);
    
    void SetValue(double value);
    double GetValue() const;
    
    void SetMinValue(double min);
    void SetMaxValue(double max);
    double GetMinValue() const { return min_; }
    double GetMaxValue() const { return max_; }
    
    void SetOnChange(std::function<void(double)> fn);
    
    virtual void SetBounds(int x, int y, int width, int height) override;
    
private:
    NSSlider* slider_ = nil;
    double min_ = 0.0;
    double max_ = 100.0;
    std::function<void(double)> onChange_;
    id target_ = nil;
};

// ============================================================================
// ScrollViewMac
// ============================================================================

class ScrollViewMac : public ControlMac {
public:
    ScrollViewMac();
    virtual ~ScrollViewMac();
    
    void Init(int x, int y, int width, int height);
    
    void SetContentView(NSView* view);
    NSView* GetContentView() const { return content_view_; }
    
    void SetHasVerticalScrollBar(bool has);
    void SetHasHorizontalScrollBar(bool has);
    void SetScrollPosition(int x, int y);
    void ScrollToBottom();
    
    virtual void SetBounds(int x, int y, int width, int height) override;
    
private:
    NSScrollView* scroll_view_ = nil;
    NSView* content_view_ = nil;
};

// ============================================================================
// ListViewMac
// ============================================================================

class ListViewMac : public ControlMac {
public:
    ListViewMac();
    virtual ~ListViewMac();
    
    void Init(int x, int y, int width, int height);
    
    void AddColumn(const std::wstring& title, int width);
    void AddRow(const std::vector<std::wstring>& cells);
    void ClearRows();
    int GetRowCount() const { return (int)rows_.size(); }
    
    int GetSelectedRow() const;
    void SelectRow(int index);
    void RemoveSelectedRow();
    
    void SetOnSelect(std::function<void(int)> fn);
    void SetOnDoubleClick(std::function<void(int)> fn);
    
    virtual void SetBounds(int x, int y, int width, int height) override;
    
    NSTableView* GetNSTableView() const { return table_; }
    
private:
    NSTableView* table_ = nil;
    NSScrollView* scroll_view_ = nil;
    std::vector<std::vector<std::wstring>> rows_;
    std::function<void(int)> onSelect_;
    std::function<void(int)> onDoubleClick_;
    id datasource_ = nil;
    id delegate_ = nil;
};

// ============================================================================
// ComboBoxMac
// ============================================================================

class ComboBoxMac : public ControlMac {
public:
    ComboBoxMac();
    virtual ~ComboBoxMac();
    
    void Init(int x, int y, int width, int height);
    
    void AddItem(const std::wstring& item);
    void InsertItem(int index, const std::wstring& item);
    void RemoveItem(int index);
    void ClearItems();
    int GetItemCount() const { return (int)items_.size(); }
    
    void SelectItem(int index);
    int GetSelectedIndex() const;
    std::wstring GetSelectedItem() const;
    
    void SetEditable(bool editable);
    bool IsEditable() const;
    
    void SetOnChange(std::function<void()> fn);
    
    virtual void SetBounds(int x, int y, int width, int height) override;
    
private:
    NSComboBox* combobox_ = nil;
    std::vector<std::wstring> items_;
    std::function<void()> onChange_;
    id target_ = nil;
};

// ============================================================================
// ProgressBarMac
// ============================================================================

class ProgressBarMac : public ControlMac {
public:
    ProgressBarMac();
    virtual ~ProgressBarMac();
    
    void Init(int x, int y, int width, int height);
    
    void SetValue(double value);
    double GetValue() const { return value_; }
    
    void SetMinValue(double min);
    void SetMaxValue(double max);
    double GetMinValue() const { return min_; }
    double GetMaxValue() const { return max_; }
    
    void SetIndeterminate(bool indeterminate);
    bool IsIndeterminate() const { return indeterminate_; }
    
    void StartAnimation();
    void StopAnimation();
    
    virtual void SetBounds(int x, int y, int width, int height) override;
    
private:
    NSProgressIndicator* progress_ = nil;
    double value_ = 0.0;
    double min_ = 0.0;
    double max_ = 100.0;
    bool indeterminate_ = false;
};

// ============================================================================
// SeparatorMac
// ============================================================================

class SeparatorMac : public ControlMac {
public:
    SeparatorMac();
    virtual ~SeparatorMac();
    
    void Init(int x, int y, int width, int height);
    void InitHorizontal(int x, int y, int width);
    void InitVertical(int x, int y, int height);
    
    void SetHorizontal(bool horizontal);
    
    virtual void SetBounds(int x, int y, int width, int height) override;
    
private:
    NSBox* separator_ = nil;
    bool horizontal_ = true;
};

// ============================================================================
// PanelMac - floating/dialog window
// ============================================================================

class PanelMac {
public:
    PanelMac();
    ~PanelMac();
    
    bool Init(int width, int height);
    
    void SetTitle(const std::wstring& title);
    std::wstring GetTitle() const { return title_; }
    
    void Show();
    void Hide();
    void Close();
    
    void SetContentView(NSView* view);
    NSView* GetContentView() const { return content_view_; }
    
    void SetAsUtilityWindow();
    void SetAsModalWindow(NSWindow* parent);
    
    NSPanel* GetNSPanel() const { return panel_; }
    
    // Run panel as sheet attached to parent, calls callback when done
    void RunAsSheet(NSWindow* parent, std::function<void()> onDismiss);
    
private:
    NSPanel* panel_ = nil;
    NSView* content_view_ = nil;
    std::wstring title_;
    std::function<void()> onDismiss_;
};

} // namespace mac
} // namespace aura

#endif // AURA_MAC
#endif // AURA_MAC_CONTROLS_H_

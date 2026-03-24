#ifndef AURA_MAC_CONTROLS_H_
#define AURA_MAC_CONTROLS_H_

// macOS-specific Aura Controls implementation
// This file only compiled on macOS with -DAURA_MAC flag

#ifdef AURA_MAC

#import <Cocoa/Cocoa.h>

#include <string>
#include "view/view_mac.h"

namespace aura {
namespace mac {

// Button control
class ButtonMac : public ViewMac {
public:
    ButtonMac();
    virtual ~ButtonMac();
    
    void SetText(const std::wstring& text);
    std::wstring GetText() const { return text_; }
    
    void SetListener(void (*onClick)()) { onClick_ = onClick; }
    
    NSButton* GetNSButton() const { return button_; }
    
    virtual void SetBounds(int x, int y, int width, int height) override;
    
private:
    NSButton* button_;
    std::wstring text_;
    void (*onClick_)();
};

// Label control
class LabelMac : public ViewMac {
public:
    LabelMac();
    LabelMac(const std::wstring& text);
    virtual ~LabelMac();
    
    void SetText(const std::wstring& text);
    std::wstring GetText() const { return text_; }
    
    void SetFont(NSFont* font);
    void SetTextColor(NSColor* color);
    void SetAlignment(NSTextAlignment align);
    
    NSTextField* GetNSLabel() const { return label_; }
    
    virtual void SetBounds(int x, int y, int width, int height) override;
    
private:
    NSTextField* label_;
    std::wstring text_;
};

// TextField control
class TextFieldMac : public ViewMac {
public:
    TextFieldMac();
    virtual ~TextFieldMac();
    
    void SetText(const std::wstring& text);
    std::wstring GetText() const;
    
    void SetPlaceholder(const std::wstring& text);
    
    void SetPasswordMode(bool password);
    bool IsPasswordMode() const { return password_; }
    
    void SetReadOnly(bool read_only);
    bool IsReadOnly() const { return read_only_; }
    
    NSTextField* GetNSTextField() const { return textfield_; }
    
    virtual void SetBounds(int x, int y, int width, int height) override;
    
private:
    NSTextField* textfield_;
    std::wstring placeholder_;
    bool password_;
    bool read_only_;
};

// Checkbox control
class CheckboxMac : public ViewMac {
public:
    CheckboxMac();
    CheckboxMac(const std::wstring& text);
    virtual ~CheckboxMac();
    
    void SetText(const std::wstring& text);
    std::wstring GetText() const { return text_; }
    
    void SetChecked(bool checked);
    bool IsChecked() const;
    
    NSButton* GetNSCheckbox() const { return checkbox_; }
    
    virtual void SetBounds(int x, int y, int width, int height) override;
    
private:
    NSButton* checkbox_;
    std::wstring text_;
};

} // namespace mac
} // namespace aura

#endif // AURA_MAC
#endif // AURA_MAC_CONTROLS_H_

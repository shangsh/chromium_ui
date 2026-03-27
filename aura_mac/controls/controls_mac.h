#ifndef AURA_MAC_CONTROLS_H_
#define AURA_MAC_CONTROLS_H_

// macOS-specific Aura Controls implementation
// This file only compiled on macOS with -DAURA_MAC flag

#ifdef AURA_MAC

#import <Cocoa/Cocoa.h>

#include <string>
#include <functional>
#include "../view/view_mac.h"

// Forward declare ObjC classes at global scope (ObjC++ requirement)
@class AuraButtonTarget;
@class AuraTextFieldTarget;

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
    id target_ = nil;  // ObjC object (AuraButtonTarget/AuraTextFieldTarget)
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
    id target_ = nil;  // ObjC object (AuraTextFieldTarget)
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
    
    virtual void SetBounds(int x, int y, int width, int height) override;
    
private:
    NSButton* checkbox_ = nil;
    std::wstring text_;
};

} // namespace mac
} // namespace aura

#endif // AURA_MAC
#endif // AURA_MAC_CONTROLS_H_

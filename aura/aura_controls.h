#ifndef AURA_CONTROLS_H_
#define AURA_CONTROLS_H_

#import <Cocoa/Cocoa.h>

#include <string>
#include "aura/aura_view.h"

namespace aura {

class ButtonListener {
public:
    virtual void OnButtonClick() = 0;
};

class Button : public View {
public:
    Button();
    virtual ~Button();
    
    void SetText(const std::wstring& text);
    std::wstring GetText() const { return text_; }
    
    void SetIcon(NSImage* icon);
    
    void SetListener(ButtonListener* listener) { listener_ = listener; }
    ButtonListener* GetListener() const { return listener_; }
    
    NSButton* native_button() const { return button_; }
    
    virtual gfx::Size GetPreferredSize() const override;
    
private:
    void CreateButton();
    
    NSButton* button_;
    std::wstring text_;
    ButtonListener* listener_;
};

class Label : public View {
public:
    Label();
    Label(const std::wstring& text);
    virtual ~Label();
    
    void SetText(const std::wstring& text);
    std::wstring GetText() const { return text_; }
    
    void SetFont(NSFont* font);
    void SetTextColor(NSColor* color);
    void SetAlignment(NSTextAlignment align);
    
    virtual gfx::Size GetPreferredSize() const override;
    
private:
    void CreateLabel();
    
    NSTextField* label_;
    std::wstring text_;
};

class TextField : public View {
public:
    TextField();
    virtual ~TextField();
    
    void SetText(const std::wstring& text);
    std::wstring GetText() const;
    
    void SetPlaceholder(const std::wstring& text);
    
    void SetPasswordMode(bool password);
    bool IsPasswordMode() const { return password_; }
    
    void SetReadOnly(bool read_only);
    bool IsReadOnly() const { return read_only_; }
    
    NSTextField* native_textfield() const { return textfield_; }
    
    virtual gfx::Size GetPreferredSize() const override;
    
private:
    void CreateTextField();
    
    NSTextField* textfield_;
    std::wstring placeholder_;
    bool password_;
    bool read_only_;
};

class Checkbox : public View {
public:
    Checkbox();
    Checkbox(const std::wstring& text);
    virtual ~Checkbox();
    
    void SetText(const std::wstring& text);
    std::wstring GetText() const { return text_; }
    
    void SetChecked(bool checked);
    bool IsChecked() const;
    
    NSButton* native_checkbox() const { return checkbox_; }
    
    virtual gfx::Size GetPreferredSize() const override;
    
private:
    NSButton* checkbox_;
    std::wstring text_;
};

} // namespace aura

#endif

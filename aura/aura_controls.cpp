#include "aura/aura_controls.h"

namespace aura {

// Button implementation
Button::Button() : button_(nil), listener_(nullptr) {
}

Button::~Button() {
    if (button_) {
        [button_ release];
    }
}

void Button::SetText(const std::wstring& text) {
    text_ = text;
    if (button_) {
        NSString* s = [NSString stringWithUTF8String:std::string(text.begin(), text.end()).c_str()];
        [button_ setTitle:s];
    }
}

void Button::SetIcon(NSImage* icon) {
    if (button_ && icon) {
        [button_ setImage:icon];
    }
}

void Button::CreateButton() {
    if (!button_) {
        button_ = [[NSButton alloc] initWithFrame:NSMakeRect(bounds_.x(), bounds_.y(), 
                                                            bounds_.width(), bounds_.height())];
        [button_ setButtonType:NSButtonTypeMomentaryPushIn];
        [button_ setBezelStyle:NSBezelStyleRounded];
        
        if (!text_.empty()) {
            NSString* s = [NSString stringWithUTF8String:std::string(text_.begin(), text_.end()).c_str()];
            [button_ setTitle:s];
        }
        
        SetNativeView(button_);
    }
}

gfx::Size Button::GetPreferredSize() const {
    if (button_) {
        NSSize size = [button_ intrinsicContentSize];
        return gfx::Size((int)size.width + 20, (int)size.height + 10);
    }
    return gfx::Size(100, 30);
}

// Label implementation
Label::Label() : label_(nil) {
}

Label::Label(const std::wstring& text) : text_(text), label_(nil) {
}

Label::~Label() {
    if (label_) {
        [label_ release];
    }
}

void Label::SetText(const std::wstring& text) {
    text_ = text;
    if (label_) {
        NSString* s = [NSString stringWithUTF8String:std::string(text.begin(), text.end()).c_str()];
        [label_ setStringValue:s];
    }
}

void Label::SetFont(NSFont* font) {
    if (label_ && font) {
        [label_ setFont:font];
    }
}

void Label::SetTextColor(NSColor* color) {
    if (label_ && color) {
        [label_ setTextColor:color];
    }
}

void Label::SetAlignment(NSTextAlignment align) {
    if (label_) {
        [label_ setAlignment:align];
    }
}

void Label::CreateLabel() {
    if (!label_) {
        label_ = [[NSTextField alloc] initWithFrame:NSMakeRect(bounds_.x(), bounds_.y(),
                                                               bounds_.width(), bounds_.height())];
        [label_ setBezeled:NO];
        [label_ setDrawsBackground:NO];
        [label_ setEditable:NO];
        [label_ setSelectable:NO];
        
        if (!text_.empty()) {
            NSString* s = [NSString stringWithUTF8String:std::string(text_.begin(), text_.end()).c_str()];
            [label_ setStringValue:s];
        }
        
        SetNativeView(label_);
    }
}

gfx::Size Label::GetPreferredSize() const {
    if (label_) {
        NSSize size = [label_ intrinsicContentSize];
        return gfx::Size((int)size.width + 10, (int)size.height + 5);
    }
    return gfx::Size(100, 20);
}

// TextField implementation
TextField::TextField() : textfield_(nil), password_(false), read_only_(false) {
}

TextField::~TextField() {
    if (textfield_) {
        [textfield_ release];
    }
}

void TextField::SetText(const std::wstring& text) {
    if (textfield_) {
        NSString* s = [NSString stringWithUTF8String:std::string(text.begin(), text.end()).c_str()];
        [textfield_ setStringValue:s];
    }
}

std::wstring TextField::GetText() const {
    if (textfield_) {
        NSString* s = [textfield_ stringValue];
        if (s) {
            std::string s8 = [s UTF8String];
            return std::wstring(s8.begin(), s8.end());
        }
    }
    return L"";
}

void TextField::SetPlaceholder(const std::wstring& text) {
    placeholder_ = text;
    if (textfield_) {
        NSString* s = [NSString stringWithUTF8String:std::string(text.begin(), text.end()).c_str()];
        [textfield_ setPlaceholderString:s];
    }
}

void TextField::SetPasswordMode(bool password) {
    password_ = password;
    if (textfield_) {
        [textfield_ setSecureTextEntry:password];
    }
}

void TextField::SetReadOnly(bool read_only) {
    read_only_ = read_only;
    if (textfield_) {
        [textfield_ setEditable:!read_only];
    }
}

void TextField::CreateTextField() {
    if (!textfield_) {
        textfield_ = [[NSTextField alloc] initWithFrame:NSMakeRect(bounds_.x(), bounds_.y(),
                                                                     bounds_.width(), bounds_.height())];
        
        if (!placeholder_.empty()) {
            NSString* s = [NSString stringWithUTF8String:std::string(placeholder_.begin(), placeholder_.end()).c_str()];
            [textfield_ setPlaceholderString:s];
        }
        
        SetNativeView(textfield_);
    }
}

gfx::Size TextField::GetPreferredSize() const {
    if (textfield_) {
        NSSize size = [textfield_ intrinsicContentSize];
        return gfx::Size((int)size.width + 20, (int)size.height + 5);
    }
    return gfx::Size(150, 24);
}

// Checkbox implementation
Checkbox::Checkbox() : checkbox_(nil) {
}

Checkbox::Checkbox(const std::wstring& text) : text_(text), checkbox_(nil) {
}

Checkbox::~Checkbox() {
    if (checkbox_) {
        [checkbox_ release];
    }
}

void Checkbox::SetText(const std::wstring& text) {
    text_ = text;
    if (checkbox_) {
        NSString* s = [NSString stringWithUTF8String:std::string(text.begin(), text.end()).c_str()];
        [checkbox_ setTitle:s];
    }
}

void Checkbox::SetChecked(bool checked) {
    if (checkbox_) {
        [checkbox_ setState:checked ? NSControlStateValueOn : NSControlStateValueOff];
    }
}

bool Checkbox::IsChecked() const {
    return checkbox_ && [checkbox_ state] == NSControlStateValueOn;
}

gfx::Size Checkbox::GetPreferredSize() const {
    if (checkbox_) {
        NSSize size = [checkbox_ intrinsicContentSize];
        return gfx::Size((int)size.width, (int)size.height);
    }
    return gfx::Size(100, 20);
}

} // namespace aura

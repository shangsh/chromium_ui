#ifdef AURA_MAC

#include "controls/controls_mac.h"

namespace aura {
namespace mac {

// Button implementation
ButtonMac::ButtonMac() : button_(nil), onClick_(nullptr) {
}

ButtonMac::~ButtonMac() {
    if (button_) {
        [button_ release];
    }
}

void ButtonMac::SetText(const std::wstring& text) {
    text_ = text;
    if (button_) {
        NSString* s = [NSString stringWithUTF8String:std::string(text.begin(), text.end()).c_str()];
        [button_ setTitle:s];
    }
}

void ButtonMac::SetBounds(int x, int y, int width, int height) {
    ViewMac::SetBounds(x, y, width, height);
    if (button_) {
        [button_ setFrame:NSMakeRect(x, y, width, height)];
    }
}

// Label implementation
LabelMac::LabelMac() : label_(nil) {
}

LabelMac::LabelMac(const std::wstring& text) : text_(text), label_(nil) {
}

LabelMac::~LabelMac() {
    if (label_) {
        [label_ release];
    }
}

void LabelMac::SetText(const std::wstring& text) {
    text_ = text;
    if (label_) {
        NSString* s = [NSString stringWithUTF8String:std::string(text.begin(), text.end()).c_str()];
        [label_ setStringValue:s];
    }
}

void LabelMac::SetFont(NSFont* font) {
    if (label_ && font) {
        [label_ setFont:font];
    }
}

void LabelMac::SetTextColor(NSColor* color) {
    if (label_ && color) {
        [label_ setTextColor:color];
    }
}

void LabelMac::SetAlignment(NSTextAlignment align) {
    if (label_) {
        [label_ setAlignment:align];
    }
}

void LabelMac::SetBounds(int x, int y, int width, int height) {
    ViewMac::SetBounds(x, y, width, height);
    if (label_) {
        [label_ setFrame:NSMakeRect(x, y, width, height)];
    }
}

// TextField implementation
TextFieldMac::TextFieldMac() : textfield_(nil), password_(false), read_only_(false) {
}

TextFieldMac::~TextFieldMac() {
    if (textfield_) {
        [textfield_ release];
    }
}

void TextFieldMac::SetText(const std::wstring& text) {
    if (textfield_) {
        NSString* s = [NSString stringWithUTF8String:std::string(text.begin(), text.end()).c_str()];
        [textfield_ setStringValue:s];
    }
}

std::wstring TextFieldMac::GetText() const {
    if (textfield_) {
        NSString* s = [textfield_ stringValue];
        if (s) {
            std::string s8 = [s UTF8String];
            return std::wstring(s8.begin(), s8.end());
        }
    }
    return L"";
}

void TextFieldMac::SetPlaceholder(const std::wstring& text) {
    placeholder_ = text;
    if (textfield_) {
        NSString* s = [NSString stringWithUTF8String:std::string(text.begin(), text.end()).c_str()];
        [textfield_ setPlaceholderString:s];
    }
}

void TextFieldMac::SetPasswordMode(bool password) {
    password_ = password;
    if (textfield_) {
        // NSTextField doesn't support setSecureTextEntry directly
        // Recreate as NSSecureTextField when entering password mode
        if (password) {
            NSRect frame = [textfield_ frame];
            NSSecureTextField* secure = [[NSSecureTextField alloc] initWithFrame:frame];
            [secure setFont:[textfield_ font]];
            [secure setPlaceholderString:[textfield_ placeholderString]];
            [textfield_ release];
            textfield_ = secure;
        }
    }
}

void TextFieldMac::SetReadOnly(bool read_only) {
    read_only_ = read_only;
    if (textfield_) {
        [textfield_ setEditable:!read_only];
    }
}

void TextFieldMac::SetBounds(int x, int y, int width, int height) {
    ViewMac::SetBounds(x, y, width, height);
    if (textfield_) {
        [textfield_ setFrame:NSMakeRect(x, y, width, height)];
    }
}

// Checkbox implementation
CheckboxMac::CheckboxMac() : checkbox_(nil) {
}

CheckboxMac::CheckboxMac(const std::wstring& text) : text_(text), checkbox_(nil) {
}

CheckboxMac::~CheckboxMac() {
    if (checkbox_) {
        [checkbox_ release];
    }
}

void CheckboxMac::SetText(const std::wstring& text) {
    text_ = text;
    if (checkbox_) {
        NSString* s = [NSString stringWithUTF8String:std::string(text.begin(), text.end()).c_str()];
        [checkbox_ setTitle:s];
    }
}

void CheckboxMac::SetChecked(bool checked) {
    if (checkbox_) {
        [checkbox_ setState:checked ? NSControlStateValueOn : NSControlStateValueOff];
    }
}

bool CheckboxMac::IsChecked() const {
    return checkbox_ && [checkbox_ state] == NSControlStateValueOn;
}

void CheckboxMac::SetBounds(int x, int y, int width, int height) {
    ViewMac::SetBounds(x, y, width, height);
    if (checkbox_) {
        [checkbox_ setFrame:NSMakeRect(x, y, width, height)];
    }
}

} // namespace mac
} // namespace aura

#endif // AURA_MAC

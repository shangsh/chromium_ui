#ifdef AURA_MAC

#import "controls/controls_mac.h"
#include <functional>
#include <string>

// ============================================================================
// ObjC Target classes - bridge native events to C++ std::function
// Declared at global scope (ObjC requires this)
// ============================================================================

@interface AuraButtonTarget : NSObject
@property (nonatomic, assign) std::function<void()>* onClickFn;
- (void)onButtonClick:(id)sender;
@end

@interface AuraTextFieldTarget : NSObject
@property (nonatomic, assign) std::function<void()>* onChangeFn;
- (void)onTextChange:(id)sender;
@end

@implementation AuraButtonTarget
@synthesize onClickFn;

- (void)onButtonClick:(id)sender {
    if (self->onClickFn && *self->onClickFn) {
        (*self->onClickFn)();
    }
}
@end

@implementation AuraTextFieldTarget
@synthesize onChangeFn;

- (void)onTextChange:(id)sender {
    if (self->onChangeFn && *self->onChangeFn) {
        (*self->onChangeFn)();
    }
}
@end

namespace aura {
namespace mac {

// ============================================================================
// ControlMac base
// ============================================================================

void ControlMac::SetVisible(bool visible) {
    if (nsview_) {
        [nsview_ setHidden:!visible];
    }
}

void ControlMac::SetEnabled(bool enabled) {
    // NSView doesn't have setEnabled; subclasses handle this
}

// ============================================================================
// ButtonMac
// ============================================================================

ButtonMac::ButtonMac() {}

ButtonMac::~ButtonMac() {
    if (button_) {
        [button_ release];
        button_ = nil;
    }
    if (target_) {
        [target_ release];
        target_ = nil;
    }
}

void ButtonMac::Init(int x, int y, int width, int height) {
    if (button_) return;
    
    button_ = [[NSButton alloc] initWithFrame:NSMakeRect(x, y, width, height)];
    [button_ setButtonType:NSButtonTypeMomentaryPushIn];
    [button_ setBezelStyle:NSBezelStyleRounded];
    [button_ setTitle:@""];
    
    // Set up click handler
    if (onClick_) {
        target_ = [[AuraButtonTarget alloc] init];
        [(AuraButtonTarget*)target_ setOnClickFn:&onClick_];
        [button_ setTarget:target_];
        [button_ setAction:@selector(onButtonClick:)];
    }
    
    nsview_ = button_;
}

void ButtonMac::SetText(const std::wstring& text) {
    text_ = text;
    if (button_) {
        NSString* s = [NSString stringWithUTF8String:std::string(text.begin(), text.end()).c_str()];
        [button_ setTitle:s];
    }
}

void ButtonMac::SetOnClick(std::function<void()> fn) {
    onClick_ = fn;
    if (button_ && fn) {
        if (!target_) {
            target_ = [[AuraButtonTarget alloc] init];
        }
        [(AuraButtonTarget*)target_ setOnClickFn:&onClick_];
        [button_ setTarget:target_];
        [button_ setAction:@selector(onButtonClick:)];
    }
}

void ButtonMac::SetBounds(int x, int y, int width, int height) {
    if (button_) {
        [button_ setFrame:NSMakeRect(x, y, width, height)];
    }
}

// ============================================================================
// LabelMac
// ============================================================================

LabelMac::LabelMac() : label_(nil) {}

LabelMac::LabelMac(const std::wstring& text) : label_(nil), text_(text) {}

LabelMac::~LabelMac() {
    if (label_) {
        [label_ release];
        label_ = nil;
    }
}

void LabelMac::Init(int x, int y, int width, int height) {
    if (label_) return;
    label_ = [[NSTextField alloc] initWithFrame:NSMakeRect(x, y, width, height)];
    [label_ setStringValue:@""];
    [label_ setBezeled:NO];
    [label_ setDrawsBackground:NO];
    [label_ setEditable:NO];
    [label_ setSelectable:NO];
    nsview_ = label_;
}

void LabelMac::Init(int x, int y, int width, int height, const std::wstring& text) {
    Init(x, y, width, height);
    SetText(text);
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
    if (label_) {
        [label_ setFrame:NSMakeRect(x, y, width, height)];
    }
}

// ============================================================================
// TextFieldMac
// ============================================================================

TextFieldMac::TextFieldMac()
    : textfield_(nil), password_(false), read_only_(false), target_(nil) {}

TextFieldMac::~TextFieldMac() {
    if (textfield_) {
        [textfield_ release];
        textfield_ = nil;
    }
    if (target_) {
        [target_ release];
        target_ = nil;
    }
}

void TextFieldMac::Init(int x, int y, int width, int height) {
    if (textfield_) return;
    
    textfield_ = [[NSTextField alloc] initWithFrame:NSMakeRect(x, y, width, height)];
    [textfield_ setStringValue:@""];
    [textfield_ setBezeled:YES];
    [textfield_ setBezelStyle:NSTextFieldSquareBezel];
    [textfield_ setEditable:YES];
    
    if (!placeholder_.empty()) {
        NSString* ph = [NSString stringWithUTF8String:std::string(placeholder_.begin(), placeholder_.end()).c_str()];
        [textfield_ setPlaceholderString:ph];
    }
    
    nsview_ = textfield_;
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
        NSString* ph = [NSString stringWithUTF8String:std::string(text.begin(), text.end()).c_str()];
        [textfield_ setPlaceholderString:ph];
    }
}

void TextFieldMac::SetPasswordMode(bool password) {
    password_ = password;
    if (textfield_) {
        if (password) {
            NSRect frame = [textfield_ frame];
            NSSecureTextField* secure = [[NSSecureTextField alloc] initWithFrame:frame];
            [secure setFont:[textfield_ font]];
            if (!placeholder_.empty()) {
                NSString* ph = [NSString stringWithUTF8String:std::string(placeholder_.begin(), placeholder_.end()).c_str()];
                [secure setPlaceholderString:ph];
            }
            [textfield_ release];
            textfield_ = secure;
            nsview_ = secure;
        }
    }
}

void TextFieldMac::SetReadOnly(bool read_only) {
    read_only_ = read_only;
    if (textfield_) {
        [textfield_ setEditable:!read_only];
    }
}

void TextFieldMac::SetOnChange(std::function<void()> fn) {
    onChange_ = fn;
    if (textfield_ && fn) {
        if (!target_) {
            target_ = [[AuraTextFieldTarget alloc] init];
        }
        [(AuraTextFieldTarget*)target_ setOnChangeFn:&onChange_];
        [textfield_ setTarget:target_];
        [textfield_ setAction:@selector(onTextChange:)];
    }
}

void TextFieldMac::SetBounds(int x, int y, int width, int height) {
    if (textfield_) {
        [textfield_ setFrame:NSMakeRect(x, y, width, height)];
    }
}

// ============================================================================
// CheckboxMac
// ============================================================================

CheckboxMac::CheckboxMac() : checkbox_(nil) {}

CheckboxMac::CheckboxMac(const std::wstring& text) : checkbox_(nil), text_(text) {}

CheckboxMac::~CheckboxMac() {
    if (checkbox_) {
        [checkbox_ release];
        checkbox_ = nil;
    }
}

void CheckboxMac::Init(int x, int y, int width, int height) {
    if (checkbox_) return;
    checkbox_ = [[NSButton alloc] initWithFrame:NSMakeRect(x, y, width, height)];
    [checkbox_ setButtonType:NSButtonTypeSwitch];
    [checkbox_ setTitle:@""];
    nsview_ = checkbox_;
}

void CheckboxMac::Init(int x, int y, int width, int height, const std::wstring& text) {
    Init(x, y, width, height);
    SetText(text);
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
    if (checkbox_) {
        return [checkbox_ state] == NSControlStateValueOn;
    }
    return false;
}

void CheckboxMac::SetBounds(int x, int y, int width, int height) {
    if (checkbox_) {
        [checkbox_ setFrame:NSMakeRect(x, y, width, height)];
    }
}

} // namespace mac
} // namespace aura

#endif // AURA_MAC

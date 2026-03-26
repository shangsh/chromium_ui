#include "aura/aura_view.h"

namespace aura {

// View implementation
View::View()
    : id_(0),
      visible_(true),
      enabled_(true),
      focusable_(false),
      parent_(nullptr),
      nsview_(nullptr),
      background_(nullptr),
      border_(nullptr) {
}

View::~View() {
    for (View* child : children_) {
        delete child;
    }
    children_.clear();
}

void View::AddChildView(View* child) {
    if (!child) return;
    children_.push_back(child);
    child->parent_ = this;
}

void View::RemoveChildView(View* child) {
    if (!child) return;
    for (auto it = children_.begin(); it != children_.end(); ++it) {
        if (*it == child) {
            children_.erase(it);
            child->parent_ = nullptr;
            return;
        }
    }
}

void View::SetBounds(int x, int y, int width, int height) {
    bounds_ = gfx::Rect(x, y, width, height);
    if (nsview_) {
        [nsview_ setFrame:NSMakeRect(x, y, width, height)];
    }
    Invalidate();
}

void View::SetPosition(int x, int y) {
    SetBounds(x, y, bounds_.width(), bounds_.height());
}

void View::SetSize(int width, int height) {
    SetBounds(bounds_.x(), bounds_.y(), width, height);
}

void View::SetVisible(bool visible) {
    visible_ = visible;
    if (nsview_) {
        [nsview_ setHidden:!visible];
    }
}

void View::SetEnabled(bool enabled) {
    enabled_ = enabled;
}

void View::SetFocusable(bool focusable) {
    focusable_ = focusable;
}

void View::RequestFocus() {
    if (nsview_ && focusable_) {
        [nsview_ becomeFirstResponder];
    }
}

void View::SetNativeView(NSView* view) {
    nsview_ = view;
    if (nsview_) {
        [nsview_ setFrame:NSMakeRect(bounds_.x(), bounds_.y(), 
                                      bounds_.width(), bounds_.height())];
    }
}

void View::OnPaint() {
    // Override in subclasses
}

void View::Invalidate() {
    if (nsview_) {
        [nsview_ setNeedsDisplay:YES];
    }
}

void View::Layout() {
    // Default: layout children
    for (View* child : children_) {
        if (child->IsVisible()) {
            child->Layout();
        }
    }
}

gfx::Size View::GetPreferredSize() const {
    return size();
}

// Background implementation
Background* Background::CreateSolidBackground(int r, int g, int b) {
    Background* bg = new Background();
    bg->color_ = [NSColor colorWithRed:r/255.0 green:g/255.0 blue:b/255.0 alpha:1.0];
    return bg;
}

Background* Background::CreateSystemColor(int color_id) {
    Background* bg = new Background();
    bg->color_ = [NSColor windowBackgroundColor];
    return bg;
}

// Border implementation
Border* Border::CreateSolidBorder(int width) {
    Border* border = new Border();
    border->width_ = width;
    return border;
}

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

void Button::SetBounds(int x, int y, int width, int height) {
    View::SetBounds(x, y, width, height);
    if (button_) {
        [button_ setFrame:NSMakeRect(x, y, width, height)];
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

void Label::SetBounds(int x, int y, int width, int height) {
    View::SetBounds(x, y, width, height);
    if (label_) {
        [label_ setFrame:NSMakeRect(x, y, width, height)];
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

void TextField::SetBounds(int x, int y, int width, int height) {
    View::SetBounds(x, y, width, height);
    if (textfield_) {
        [textfield_ setFrame:NSMakeRect(x, y, width, height)];
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

void Checkbox::SetBounds(int x, int y, int width, int height) {
    View::SetBounds(x, y, width, height);
    if (checkbox_) {
        [checkbox_ setFrame:NSMakeRect(x, y, width, height)];
    }
}

gfx::Size Checkbox::GetPreferredSize() const {
    if (checkbox_) {
        NSSize size = [checkbox_ intrinsicContentSize];
        return gfx::Size((int)size.width, (int)size.height);
    }
    return gfx::Size(100, 20);
}

} // namespace aura

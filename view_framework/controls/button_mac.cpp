#include "view_framework/controls/button_mac.h"

#include <string>

namespace view
{

Button::Button()
    : button_(nil),
      state_(BUTTON_STATE_NORMAL),
      listener_(nullptr)
{
}

Button::~Button()
{
    if (button_)
    {
        [button_ release];
        button_ = nil;
    }
}

void Button::SetText(const std::wstring& text)
{
    text_ = text;
    if (button_)
    {
        std::string text8(text.begin(), text.end());
        [button_ setTitle:[NSString stringWithUTF8String:text8.c_str()]];
    }
}

void Button::SetIcon(NSImage* icon)
{
    if (button_)
    {
        [button_ setImage:icon];
    }
}

void Button::SetState(ButtonState state)
{
    state_ = state;
    switch (state)
    {
        case BUTTON_STATE_NORMAL:
            // Normal appearance
            break;
        case BUTTON_STATE_HOVERED:
            // Highlighted
            break;
        case BUTTON_STATE_PRESSED:
            // Pressed appearance
            break;
        case BUTTON_STATE_DISABLED:
            // Disabled appearance
            break;
        default:
            break;
    }
}

void Button::SetEnabled(bool enabled)
{
    View::SetEnabled(enabled);
    if (button_)
    {
        [button_ setEnabled:enabled];
    }
    SetState(enabled ? BUTTON_STATE_NORMAL : BUTTON_STATE_DISABLED);
}

gfx::Size Button::GetPreferredSize() const
{
    if (button_)
    {
        NSSize size = [button_ intrinsicContentSize];
        return gfx::Size(static_cast<int>(size.width), static_cast<int>(size.height));
    }
    return gfx::Size(100, 30);
}

void Button::ViewHierarchyChanged(bool is_add, View* parent, View* child)
{
    if (is_add && !button_)
    {
        button_ = [[NSButton alloc] initWithFrame:NSMakeRect(bounds_.x(), bounds_.y(), 
                                                             bounds_.width(), bounds_.height())];
        [button_ setButtonType:NSButtonTypeMomentaryPushIn];
        [button_ setBezelStyle:NSBezelStyleRounded];
        
        if (!text_.empty())
        {
            std::string text8(text_.begin(), text_.end());
            [button_ setTitle:[NSString stringWithUTF8String:text8.c_str()]];
        }
        
        SetNativeView(button_);
    }
    View::ViewHierarchyChanged(is_add, parent, child);
}

} // namespace view

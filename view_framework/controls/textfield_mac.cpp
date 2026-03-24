#include "view_framework/controls/textfield_mac.h"

#include <string>

namespace view
{

TextField::TextField()
    : text_field_(nil),
      password_(false),
      read_only_(false),
      controller_(nullptr)
{
}

TextField::~TextField()
{
    if (text_field_)
    {
        [text_field_ release];
        text_field_ = nil;
    }
}

void TextField::SetText(const std::wstring& text)
{
    if (text_field_)
    {
        std::string text8(text.begin(), text.end());
        [text_field_ setStringValue:[NSString stringWithUTF8String:text8.c_str()]];
    }
}

std::wstring TextField::GetText() const
{
    if (text_field_)
    {
        NSString* str = [text_field_ stringValue];
        if (str)
        {
            std::string text8 = [str UTF8String];
            return std::wstring(text8.begin(), text8.end());
        }
    }
    return L"";
}

void TextField::SetPlaceholderText(const std::wstring& text)
{
    placeholder_ = text;
    if (text_field_)
    {
        std::string text8(text.begin(), text.end());
        [text_field_ setPlaceholderString:[NSString stringWithUTF8String:text8.c_str()]];
    }
}

void TextField::SetPasswordMode(bool password)
{
    password_ = password;
    if (text_field_)
    {
        [text_field_ setSecureTextEntry:password];
    }
}

void TextField::SetReadOnly(bool read_only)
{
    read_only_ = read_only;
    if (text_field_)
    {
        [text_field_ setEditable:!read_only];
    }
}

gfx::Size TextField::GetPreferredSize() const
{
    if (text_field_)
    {
        NSSize size = [text_field_ intrinsicContentSize];
        return gfx::Size(static_cast<int>(size.width) + 10, 
                        static_cast<int>(size.height) + 5);
    }
    return gfx::Size(150, 24);
}

void TextField::SetEnabled(bool enabled)
{
    View::SetEnabled(enabled);
    if (text_field_)
    {
        [text_field_ setEnabled:enabled];
    }
}

} // namespace view

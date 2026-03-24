#include "view_framework/controls/label_mac.h"

namespace view
{

Label::Label()
    : label_(nil),
      multi_line_(false)
{
}

Label::Label(const std::wstring& text)
    : text_(text),
      label_(nil),
      multi_line_(false)
{
}

Label::~Label()
{
    if (label_)
    {
        [label_ release];
        label_ = nil;
    }
}

void Label::SetText(const std::wstring& text)
{
    text_ = text;
    if (label_)
    {
        std::string text8(text.begin(), text.end());
        [label_ setStringValue:[NSString stringWithUTF8String:text8.c_str()]];
    }
}

void Label::SetFont(NSFont* font)
{
    if (label_ && font)
    {
        [label_ setFont:font];
    }
}

void Label::SetTextColor(NSColor* color)
{
    if (label_ && color)
    {
        [label_ setTextColor:color];
    }
}

void Label::SetHorizontalAlignment(NSTextAlignment alignment)
{
    if (label_)
    {
        [label_ setAlignment:alignment];
    }
}

void Label::SetVerticalAlignment(int alignment)
{
    // macOS doesn't have vertical alignment for NSTextField like Windows
}

void Label::SetMultiLine(bool multi_line)
{
    multi_line_ = multi_line;
    if (label_)
    {
        // For multi-line, use NSTextView or cell wrapping
    }
}

gfx::Size Label::GetPreferredSize() const
{
    if (label_)
    {
        NSSize size = [label_ intrinsicContentSize];
        return gfx::Size(static_cast<int>(size.width) + 10, 
                        static_cast<int>(size.height) + 5);
    }
    return gfx::Size(100, 20);
}

} // namespace view

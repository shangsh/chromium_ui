#ifndef VIEW_FRAMEWORK_CONTROLS_LABEL_MAC_H_
#define VIEW_FRAMEWORK_CONTROLS_LABEL_MAC_H_

#import <Cocoa/Cocoa.h>

#include <string>
#include "view_framework/view_mac.h"

namespace view
{

class Label : public View
{
public:
    Label();
    Label(const std::wstring& text);
    virtual ~Label();

    // Text
    void SetText(const std::wstring& text);
    std::wstring GetText() const { return text_; }

    // Font
    void SetFont(NSFont* font);

    // Text color
    void SetTextColor(NSColor* color);

    // Alignment
    void SetHorizontalAlignment(NSTextAlignment alignment);
    void SetVerticalAlignment(int alignment);

    // Multi-line
    void SetMultiLine(bool multi_line);
    bool IsMultiLine() const { return multi_line_; }

    // Override
    virtual gfx::Size GetPreferredSize() const override;

private:
    NSTextField* label_;
    std::wstring text_;
    bool multi_line_;
};

} // namespace view

#endif // VIEW_FRAMEWORK_CONTROLS_LABEL_MAC_H_

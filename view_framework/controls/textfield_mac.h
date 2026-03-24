#ifndef VIEW_FRAMEWORK_CONTROLS_TEXTFIELD_MAC_H_
#define VIEW_FRAMEWORK_CONTROLS_TEXTFIELD_MAC_H_

#import <Cocoa/Cocoa.h>

#include <string>
#include "view_framework/view_mac.h"

namespace view
{

class TextfieldController;

class TextField : public View
{
public:
    TextField();
    virtual ~TextField();

    // Text
    void SetText(const std::wstring& text);
    std::wstring GetText() const;

    // Placeholder
    void SetPlaceholderText(const std::wstring& text);
    std::wstring GetPlaceholderText() const { return placeholder_; }

    // Password mode
    void SetPasswordMode(bool password);
    bool IsPasswordMode() const { return password_; }

    // Read-only
    void SetReadOnly(bool read_only);
    bool IsReadOnly() const { return read_only_; }

    // Controller
    void SetController(TextfieldController* controller) { controller_ = controller; }
    TextfieldController* GetController() const { return controller_; }

    // Native
    NSTextField* native_textfield() const { return text_field_; }

    // Override
    virtual gfx::Size GetPreferredSize() const override;
    virtual void SetEnabled(bool enabled) override;

private:
    NSTextField* text_field_;
    std::wstring placeholder_;
    bool password_;
    bool read_only_;
    TextfieldController* controller_;
};

class TextfieldController
{
public:
    virtual void TextfieldChanged(TextField* textfield) {}
    virtual void TextfieldSubmitEditing(TextField* textfield) {}
    virtual void TextfieldFocusChange(TextField* textfield, bool has_focus) {}

protected:
    virtual ~TextfieldController() {}
};

} // namespace view

#endif // VIEW_FRAMEWORK_CONTROLS_TEXTFIELD_MAC_H_

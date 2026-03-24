#ifndef VIEW_FRAMEWORK_CONTROLS_BUTTON_MAC_H_
#define VIEW_FRAMEWORK_CONTROLS_BUTTON_MAC_H_

#import <Cocoa/Cocoa.h>

#include <string>
#include "gfx/rect.h"
#include "view_framework/view_mac.h"

namespace view
{

class ButtonListener;

class Button : public View
{
public:
    Button();
    virtual ~Button();

    // Set button text
    void SetText(const std::wstring& text);
    std::wstring GetText() const { return text_; }

    // Set icon
    void SetIcon(NSImage* icon);

    // Button state
    enum ButtonState {
        BUTTON_STATE_NORMAL = 0,
        BUTTON_STATE_HOVERED,
        BUTTON_STATE_PRESSED,
        BUTTON_STATE_DISABLED,
        BUTTON_STATE_COUNT
    };

    void SetState(ButtonState state);
    ButtonState state() const { return state_; }

    // Listener
    void set_listener(ButtonListener* listener) { listener_ = listener; }
    ButtonListener* listener() const { return listener_; }

    // Events
    virtual void OnClick() {}
    virtual void OnBlur() {}

    // NSButton access
    NSButton* native_button() const { return button_; }

    // Override View
    virtual void SetEnabled(bool enabled) override;
    virtual gfx::Size GetPreferredSize() const override;

protected:
    virtual void ViewHierarchyChanged(bool is_add, View* parent, View* child) override;

private:
    NSButton* button_;
    std::wstring text_;
    ButtonState state_;
    ButtonListener* listener_;
};

class ButtonListener
{
public:
    virtual void ButtonPressed(Button* sender) = 0;
    virtual void ButtonClicked(Button* sender) {}

protected:
    virtual ~ButtonListener() {}
};

} // namespace view

#endif // VIEW_FRAMEWORK_CONTROLS_BUTTON_MAC_H_

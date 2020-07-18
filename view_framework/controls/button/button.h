
#ifndef __button_h__
#define __button_h__

#pragma once

#include "../../view.h"

namespace view
{

    class Button;
    class Event;

    // An interface implemented by an object to let it know that a button was
    // pressed.
    class ButtonListener
    {
    public:
        virtual void ButtonPressed(Button* sender, const Event& event) = 0;

    protected:
        virtual ~ButtonListener() {}
    };

    // A View representing a button. Depending on the specific type, the button
    // could be implemented by a native control or custom rendered.
    class Button : public View
    {
    public:
        virtual ~Button();

        void SetTooltipText(const std::wstring& tooltip_text);

        int tag() const { return tag_; }
        void set_tag(int tag) { tag_ = tag; }

        int mouse_event_flags() const { return mouse_event_flags_; }

        void SetAccessibleKeyboardShortcut(const std::wstring& shortcut);

        // Overridden from View:
        virtual bool GetTooltipText(const gfx::Point& p, std::wstring* tooltip);
        virtual std::wstring GetAccessibleKeyboardShortcut();
        virtual AccessibilityTypes::Role GetAccessibleRole();

    protected:
        // Construct the Button with a Listener. The listener can be NULL. This can be
        // true of buttons that don't have a listener - e.g. menubuttons where there's
        // no default action and checkboxes.
        explicit Button(ButtonListener* listener);

        // Cause the button to notify the listener that a click occurred.
        virtual void NotifyClick(const Event& event);

        // The button's listener. Notified when clicked.
        ButtonListener* listener_;

    private:
        // The text shown in a tooltip.
        std::wstring tooltip_text_;

        // Accessibility data.
        std::wstring accessible_shortcut_;

        // The id tag associated with this button. Used to disambiguate buttons in
        // the ButtonListener implementation.
        int tag_;

        // Event flags present when the button was clicked.
        int mouse_event_flags_;

        DISALLOW_COPY_AND_ASSIGN(Button);
    };

} //namespace view

#endif //__button_h__
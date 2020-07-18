
#ifndef __native_button_wrapper_h__
#define __native_button_wrapper_h__

#pragma once

namespace view
{

    class Checkbox;
    class NativeButton;
    class RadioButton;
    class View;

    // A specialization of NativeControlWrapper that hosts a platform-native button.
    class NativeButtonWrapper
    {
    public:
        // Updates the native button's label from the state stored in its associated
        // NativeButton.
        virtual void UpdateLabel() = 0;

        // Updates the native button's label font from the state stored in its
        // associated NativeButton.
        virtual void UpdateFont() = 0;

        // Updates the native button's enabled state from the state stored in its
        // associated NativeButton.
        virtual void UpdateEnabled() = 0;

        // Updates the native button's default state from the state stored in its
        // associated NativeButton.
        virtual void UpdateDefault() = 0;

        // Updates the native button's checked state from the state stored in its
        // associated NativeCheckbox. Valid only for checkboxes and radio buttons.
        virtual void UpdateChecked() {}

        // Update the native button's accessible name to the accessible name of
        // the associated NativeButton.
        virtual void UpdateAccessibleName() {}

        // Shows the pushed state for the button if |pushed| is true.
        virtual void SetPushed(bool pushed) {}

        // Retrieves the views::View that hosts the native control.
        virtual View* GetView() = 0;

        // Sets the focus to the button.
        virtual void SetFocus() = 0;

        // Returns true if the wrapped NativeButton supplies its own label, false if
        // the caller needs to provide one.
        virtual bool UsesNativeLabel() const = 0;

        // Returns true if the wrapped NativeRadioButton supplies its own grouping
        // mechanism, or false if the radio button needs to provide one.
        virtual bool UsesNativeRadioButtonGroup() const = 0;

        // Returns a handle to the underlying native view for testing.
        virtual HWND GetTestingHandle() const = 0;

        // Return the width of the button. Used for fixed size buttons (checkboxes and
        // radio buttons) only.
        static int GetFixedWidth();

        // Creates an appropriate NativeButtonWrapper for the platform.
        static NativeButtonWrapper* CreateNativeButtonWrapper(NativeButton* button);
        static NativeButtonWrapper* CreateCheckboxWrapper(Checkbox* checkbox);
        static NativeButtonWrapper* CreateRadioButtonWrapper(
            RadioButton* radio_button);

    protected:
        virtual ~NativeButtonWrapper() {}
    };

} //namespace view

#endif //__native_button_wrapper_h__
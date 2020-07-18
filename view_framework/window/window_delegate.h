

#ifndef __window_delegate_h__
#define __window_delegate_h__

#pragma once

#include <string>

#include "base/scoped_ptr.h"

#include "gfx/bitmap.h"

#include "../accessibility/accessibility_types.h"

namespace gfx
{
    class Rect;
}

namespace view
{

    class ClientView;
    class DialogDelegate;
    class View;
    class Window;

    ///////////////////////////////////////////////////////////////////////////////
    //
    // WindowDelegate
    //
    //  WindowDelegate is an interface implemented by objects that wish to show a
    //  Window. The window that is displayed uses this interface to determine how
    //  it should be displayed and notify the delegate object of certain events.
    //
    ///////////////////////////////////////////////////////////////////////////////
    class WindowDelegate
    {
    public:
        WindowDelegate();
        virtual ~WindowDelegate();

        virtual DialogDelegate* AsDialogDelegate() { return NULL; }

        // Returns true if the window can ever be resized.
        virtual bool CanResize() const
        {
            return false;
        }

        // Returns true if the window can ever be maximized.
        virtual bool CanMaximize() const
        {
            return false;
        }

        // Returns true if the dialog should be displayed modally to the window that
        // opened it. Only windows with WindowType == DIALOG can be modal.
        virtual bool IsModal() const
        {
            return false;
        }

        virtual AccessibilityTypes::Role accessible_role() const
        {
            return AccessibilityTypes::ROLE_WINDOW;
        }

        virtual AccessibilityTypes::State accessible_state() const
        {
            return 0;
        }

        // Returns the title to be read with screen readers.
        virtual std::wstring GetAccessibleWindowTitle() const
        {
            return GetWindowTitle();
        }

        // Returns the text to be displayed in the window title.
        virtual std::wstring GetWindowTitle() const
        {
            return L"";
        }

        // Returns the view that should have the focus when the dialog is opened.  If
        // NULL no view is focused.
        virtual View* GetInitiallyFocusedView() { return NULL; }

        // Returns true if the window should show a title in the title bar.
        virtual bool ShouldShowWindowTitle() const
        {
            return true;
        }

        // Returns true if the window's client view wants a client edge.
        virtual bool ShouldShowClientEdge() const
        {
            return true;
        }

        // Returns the app icon for the window. On Windows, this is the ICON_BIG used
        // in Alt-Tab list and Win7's taskbar.
        virtual gfx::Bitmap GetWindowAppIcon();

        // Returns the icon to be displayed in the window.
        virtual gfx::Bitmap GetWindowIcon();

        // Returns true if a window icon should be shown.
        virtual bool ShouldShowWindowIcon() const
        {
            return false;
        }

        // Execute a command in the window's controller. Returns true if the command
        // was handled, false if it was not.
        virtual bool ExecuteWindowsCommand(int command_id) { return false; }

        // Returns the window's name identifier. Used to identify this window for
        // state restoration.
        virtual std::wstring GetWindowName() const
        {
            return std::wstring();
        }

        // Saves the window's bounds and maximized states. By default this uses the
        // process' local state keyed by window name (See GetWindowName above). This
        // behavior can be overridden to provide additional functionality.
        virtual void SaveWindowPlacement(const gfx::Rect& bounds, bool maximized);

        // Retrieves the window's bounds and maximized states.
        // This behavior can be overridden to provide additional functionality.
        virtual bool GetSavedWindowBounds(gfx::Rect* bounds) const;
        virtual bool GetSavedMaximizedState(bool* maximized) const;

        // Returns true if the window's size should be restored. If this is false,
        // only the window's origin is restored and the window is given its
        // preferred size.
        // Default is true.
        virtual bool ShouldRestoreWindowSize() const;

        // Called when the window closes.
        virtual void WindowClosing() {}

        // Called when the window is destroyed. No events must be sent or received
        // after this point. The delegate can use this opportunity to delete itself at
        // this time if necessary.
        virtual void DeleteDelegate() {}

        // Returns the View that is contained within this Window.
        virtual View* GetContentsView()
        {
            return NULL;
        }

        // Called by the Window to create the Client View used to host the contents
        // of the window.
        virtual ClientView* CreateClientView(Window* window);

        Window* window() const { return window_; }

    private:
        friend class WindowWin;
        // The Window this delegate is bound to. Weak reference.
        Window* window_;
    };

} //namespace view

#endif //__window_delegate_h__
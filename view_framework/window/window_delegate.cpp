
#include "window_delegate.h"

#include "../view_delegate.h"
#include "client_view.h"
#include "window.h"

namespace view
{

    WindowDelegate::WindowDelegate() : window_(NULL) {}

    WindowDelegate::~WindowDelegate() {}

    gfx::Bitmap WindowDelegate::GetWindowAppIcon()
    {
        // Use the window icon as app icon by default.
        return GetWindowIcon();
    }

    // Returns the icon to be displayed in the window.
    gfx::Bitmap WindowDelegate::GetWindowIcon()
    {
        return gfx::Bitmap();
    }

    void WindowDelegate::SaveWindowPlacement(const gfx::Rect& bounds,
        bool maximized)
    {
        std::wstring window_name = GetWindowName();
        if(!ViewDelegate::view_delegate || window_name.empty())
        {
            return;
        }

        ViewDelegate::view_delegate->SaveWindowPlacement(
            window_name, bounds, maximized);
    }

    bool WindowDelegate::GetSavedWindowBounds(gfx::Rect* bounds) const
    {
        std::wstring window_name = GetWindowName();
        if(!ViewDelegate::view_delegate || window_name.empty())
        {
            return false;
        }

        return ViewDelegate::view_delegate->GetSavedWindowBounds(
            window_name, bounds);
    }

    bool WindowDelegate::GetSavedMaximizedState(bool* maximized) const
    {
        std::wstring window_name = GetWindowName();
        if(!ViewDelegate::view_delegate || window_name.empty())
        {
            return false;
        }

        return ViewDelegate::view_delegate->GetSavedMaximizedState(
            window_name, maximized);
    }

    bool WindowDelegate::ShouldRestoreWindowSize() const
    {
        return true;
    }

    ClientView* WindowDelegate::CreateClientView(Window* window)
    {
        return new ClientView(window, GetContentsView());
    }

} //namespace view

#include "windows_util.h"

#include "base/logging.h"
#include "base/rtl.h"
#include "base/scoped_handle.h"
#include "base/scoped_native_library.h"
#include "base/win_util.h"
#include "base/windows_version.h"

#include "dwmapi_wrapper.h"

namespace view
{

    bool IsWindowActive(HWND hwnd)
    {
        WINDOWINFO info;
        return ::GetWindowInfo(hwnd, &info) &&
            ((info.dwWindowStatus&WS_ACTIVECAPTION)!=0);
    }

    gfx::Rect GetMonitorBoundsForRect(const gfx::Rect& rect)
    {
        RECT p_rect = rect.ToRECT();
        HMONITOR monitor = MonitorFromRect(&p_rect, MONITOR_DEFAULTTONEAREST);
        if(monitor)
        {
            MONITORINFO mi = {0};
            mi.cbSize = sizeof(mi);
            GetMonitorInfo(monitor, &mi);
            return gfx::Rect(mi.rcWork);
        }
        NOTREACHED();
        return gfx::Rect();
    }

    gfx::Font GetWindowTitleFont()
    {
        NONCLIENTMETRICS ncm;
        base::GetNonClientMetrics(&ncm);
        base::AdjustUIFont(&(ncm.lfCaptionFont));
        base::ScopedHFONT caption_font(CreateFontIndirect(&(ncm.lfCaptionFont)));
        return gfx::Font(caption_font);
    }

    // Adjust the window to fit, returning true if the window was resized or moved.
    static bool AdjustWindowToFit(HWND hwnd, const RECT& bounds)
    {
        // Get the monitor.
        HMONITOR hmon = MonitorFromRect(&bounds, MONITOR_DEFAULTTONEAREST);
        if(!hmon)
        {
            NOTREACHED() << "Unable to find default monitor";
            // No monitor available.
            return false;
        }

        MONITORINFO mi;
        mi.cbSize = sizeof(mi);
        GetMonitorInfo(hmon, &mi);
        gfx::Rect window_rect(bounds);
        gfx::Rect monitor_rect(mi.rcWork);
        gfx::Rect new_window_rect = window_rect.AdjustToFit(monitor_rect);
        if(!new_window_rect.Equals(window_rect))
        {
            // Window doesn't fit on monitor, move and possibly resize.
            SetWindowPos(hwnd, 0, new_window_rect.x(), new_window_rect.y(),
                new_window_rect.width(), new_window_rect.height(),
                SWP_NOACTIVATE|SWP_NOZORDER);
            return true;
        }
        else
        {
            return false;
        }
    }

    void AdjustWindowToFit(HWND hwnd)
    {
        // Get the window bounds.
        RECT r;
        GetWindowRect(hwnd, &r);
        AdjustWindowToFit(hwnd, r);
    }

    void CenterAndSizeWindow(HWND parent, HWND window, const SIZE& pref,
        bool pref_is_client)
    {
        DCHECK(window && pref.cx>0 && pref.cy>0);
        // Calculate the ideal bounds.
        RECT window_bounds;
        RECT center_bounds = {0};
        if(parent)
        {
            // If there is a parent, center over the parents bounds.
            ::GetWindowRect(parent, &center_bounds);
        }
        else
        {
            // No parent. Center over the monitor the window is on.
            HMONITOR monitor = MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST);
            if(monitor)
            {
                MONITORINFO mi = {0};
                mi.cbSize = sizeof(mi);
                GetMonitorInfo(monitor, &mi);
                center_bounds = mi.rcWork;
            }
            else
            {
                NOTREACHED() << "Unable to get default monitor";
            }
        }
        window_bounds.left = center_bounds.left +
            (center_bounds.right - center_bounds.left - pref.cx) / 2;
        window_bounds.right = window_bounds.left + pref.cx;
        window_bounds.top = center_bounds.top +
            (center_bounds.bottom - center_bounds.top - pref.cy) / 2;
        window_bounds.bottom = window_bounds.top + pref.cy;

        // If we're centering a child window, we are positioning in client
        // coordinates, and as such we need to offset the target rectangle by the
        // position of the parent window.
        if(::GetWindowLong(window, GWL_STYLE) & WS_CHILD)
        {
            DCHECK(parent && ::GetParent(window)==parent);
            POINT topleft = { window_bounds.left, window_bounds.top };
            ::MapWindowPoints(HWND_DESKTOP, parent, &topleft, 1);
            window_bounds.left = topleft.x;
            window_bounds.top = topleft.y;
            window_bounds.right = window_bounds.left + pref.cx;
            window_bounds.bottom = window_bounds.top + pref.cy;
        }

        // Get the WINDOWINFO for window. We need the style to calculate the size
        // for the window.
        WINDOWINFO win_info = { 0 };
        win_info.cbSize = sizeof(WINDOWINFO);
        GetWindowInfo(window, &win_info);

        // Calculate the window size needed for the content size.

        if(!pref_is_client ||
            AdjustWindowRectEx(&window_bounds, win_info.dwStyle, FALSE,
            win_info.dwExStyle))
        {
            if(!AdjustWindowToFit(window, window_bounds))
            {
                // The window fits, reset the bounds.
                SetWindowPos(window, 0, window_bounds.left, window_bounds.top,
                    window_bounds.right - window_bounds.left,
                    window_bounds.bottom - window_bounds.top,
                    SWP_NOACTIVATE|SWP_NOZORDER);
            } // else case, AdjustWindowToFit set the bounds for us.
        }
        else
        {
            NOTREACHED() << "Unable to adjust window to fit";
        }
    }

    bool EdgeHasTopmostAutoHideTaskbar(UINT edge, HMONITOR monitor)
    {
        APPBARDATA taskbar_data = { 0 };
        taskbar_data.cbSize = sizeof APPBARDATA;
        taskbar_data.uEdge = edge;
        HWND taskbar = reinterpret_cast<HWND>(SHAppBarMessage(ABM_GETAUTOHIDEBAR,
            &taskbar_data));
        return ::IsWindow(taskbar) && (monitor!=NULL) &&
            (MonitorFromWindow(taskbar, MONITOR_DEFAULTTONULL)==monitor) &&
            (GetWindowLong(taskbar, GWL_EXSTYLE)&WS_EX_TOPMOST);
    }

    void EnsureRectIsVisibleInRect(const gfx::Rect& parent_rect,
        gfx::Rect* child_rect, int padding)
    {
        DCHECK(child_rect);

        // We use padding here because it allows some of the original web page to
        // bleed through around the edges.
        int twice_padding = padding * 2;

        // FIRST, clamp width and height so we don't open child windows larger than
        // the containing parent.
        if(child_rect->width() > (parent_rect.width()+twice_padding))
        {
            child_rect->set_width(__max(0, parent_rect.width() - twice_padding));
        }
        if(child_rect->height() > parent_rect.height() + twice_padding)
        {
            child_rect->set_height(__max(0, parent_rect.height() - twice_padding));
        }

        // SECOND, clamp x,y position to padding,padding so we don't position child
        // windows in hyperspace.
        // TODO(mpcomplete): I don't see what the second check in each 'if' does that
        // isn't handled by the LAST set of 'ifs'.  Maybe we can remove it.
        if(child_rect->x()<parent_rect.x() || child_rect->x()>parent_rect.right())
        {
            child_rect->set_x(parent_rect.x() + padding);
        }
        if(child_rect->y()<parent_rect.y() || child_rect->y()>parent_rect.bottom())
        {
            child_rect->set_y(parent_rect.y() + padding);
        }

        // LAST, nudge the window back up into the client area if its x,y position is
        // within the parent bounds but its width/height place it off-screen.
        if(child_rect->bottom() > parent_rect.bottom())
        {
            child_rect->set_y(parent_rect.bottom() - child_rect->height() - padding);
        }
        if(child_rect->right() > parent_rect.right())
        {
            child_rect->set_x(parent_rect.right() - child_rect->width() - padding);
        }
    }

    void SetChildBounds(HWND child_window, HWND parent_window,
        HWND insert_after_window, const gfx::Rect& bounds,
        int padding, unsigned long flags)
    {
        DCHECK(IsWindow(child_window));

        // First figure out the bounds of the parent.
        RECT parent_rect = { 0 };
        if(parent_window)
        {
            GetClientRect(parent_window, &parent_rect);
        }
        else
        {
            // If there is no parent, we consider the bounds of the monitor the window
            // is on to be the parent bounds.

            // If the child_window isn't visible yet and we've been given a valid,
            // visible insert after window, use that window to locate the correct
            // monitor instead.
            HWND window = child_window;
            if(!IsWindowVisible(window) && IsWindow(insert_after_window) &&
                IsWindowVisible(insert_after_window))
            {
                window = insert_after_window;
            }

            POINT window_point = { bounds.x(), bounds.y() };
            HMONITOR monitor = MonitorFromPoint(window_point,
                MONITOR_DEFAULTTONEAREST);
            if(monitor)
            {
                MONITORINFO mi = {0};
                mi.cbSize = sizeof(mi);
                GetMonitorInfo(monitor, &mi);
                parent_rect = mi.rcWork;
            }
            else
            {
                NOTREACHED() << "Unable to get default monitor";
            }
        }

        gfx::Rect actual_bounds = bounds;
        EnsureRectIsVisibleInRect(gfx::Rect(parent_rect), &actual_bounds, padding);

        SetWindowPos(child_window, insert_after_window, actual_bounds.x(),
            actual_bounds.y(), actual_bounds.width(),
            actual_bounds.height(), flags);
    }

    bool ShouldUseVistaFrame()
    {
        if(base::GetWinVersion() < base::WINVERSION_VISTA)
        {
            return false;
        }

        BOOL enabled = FALSE;
        DwmapiWrapper::instance()->DwmIsCompositionEnabled(&enabled);

        return !!enabled;
    }

    void GetDefaultWindowMask(const gfx::Size& size, gfx::Path* window_mask)
    {
        // Redefine the window visible region for the new size.
        window_mask->MoveTo(0, 3);
        window_mask->LineTo(1, 2);
        window_mask->LineTo(1, 1);
        window_mask->LineTo(2, 1);
        window_mask->LineTo(3, 0);

        window_mask->LineTo(size.width()-3, 0);
        window_mask->LineTo(size.width()-2, 1);
        window_mask->LineTo(size.width()-1, 1);
        window_mask->LineTo(size.width()-1, 2);
        window_mask->LineTo(size.width(), 3);

        window_mask->LineTo(size.width(), size.height()-3);
        window_mask->LineTo(size.width()-1, size.height()-3);
        window_mask->LineTo(size.width()-1, size.height()-1);
        window_mask->LineTo(size.width()-3, size.height()-2);
        window_mask->LineTo(size.width()-3, size.height());

        window_mask->LineTo(3, size.height());
        window_mask->LineTo(2, size.height()-2);
        window_mask->LineTo(1, size.height()-1);
        window_mask->LineTo(1, size.height()-3);
        window_mask->LineTo(0, size.height()-3);

        window_mask->Close();
    }

} //namespace view
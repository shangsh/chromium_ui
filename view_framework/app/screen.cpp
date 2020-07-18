
#include "screen.h"

#include <windows.h>

namespace view
{

    // static
    gfx::Point Screen::GetCursorScreenPoint()
    {
        POINT pt;
        GetCursorPos(&pt);
        return gfx::Point(pt);
    }

    // static
    gfx::Rect Screen::GetMonitorWorkAreaNearestWindow(HWND window)
    {
        MONITORINFO monitor_info;
        monitor_info.cbSize = sizeof(monitor_info);
        GetMonitorInfo(MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST),
            &monitor_info);
        return gfx::Rect(monitor_info.rcWork);
    }

    // static
    gfx::Rect Screen::GetMonitorAreaNearestWindow(HWND window)
    {
        MONITORINFO monitor_info;
        monitor_info.cbSize = sizeof(monitor_info);
        GetMonitorInfo(MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST),
            &monitor_info);
        return gfx::Rect(monitor_info.rcMonitor);
    }

    // static
    gfx::Rect Screen::GetMonitorAreaNearestPoint(const gfx::Point& point)
    {
        POINT initial_loc = { point.x(), point.y() };
        HMONITOR monitor = MonitorFromPoint(initial_loc, MONITOR_DEFAULTTONEAREST);
        if(!monitor)
        {
            return gfx::Rect();
        }

        MONITORINFO mi = { 0 };
        mi.cbSize = sizeof(mi);
        GetMonitorInfo(monitor, &mi);
        return gfx::Rect(mi.rcMonitor);
    }

    HWND Screen::GetWindowAtCursorScreenPoint()
    {
        POINT location;
        return GetCursorPos(&location) ? WindowFromPoint(location) : NULL;
    }

} //namespace view
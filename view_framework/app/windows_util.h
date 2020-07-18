
#ifndef __windows_util_h__
#define __windows_util_h__

#include <windows.h>

#include "gfx/font.h"
#include "gfx/rect.h"
#include "gfx/size.h"
#include "gfx/path.h"

namespace view
{

    const int kAutoHideTaskbarThicknessPx = 2;

    // Returns whether the specified window is the current active window.
    bool IsWindowActive(HWND hwnd);

    // Returns the bounds for the monitor that contains the largest area of
    // intersection with the specified rectangle.
    gfx::Rect GetMonitorBoundsForRect(const gfx::Rect& rect);

    // Returns the system set window title font.
    gfx::Font GetWindowTitleFont();

    // If the window does not fit on the default monitor, it is moved and possibly
    // resized appropriately.
    void AdjustWindowToFit(HWND hwnd);

    // Sizes the window to have a client or window size (depending on the value of
    // |pref_is_client|) of pref, then centers the window over parent, ensuring the
    // window fits on screen.
    void CenterAndSizeWindow(HWND parent, HWND window, const SIZE& pref,
        bool pref_is_client);

    // Returns true if edge |edge| (one of ABE_LEFT, TOP, RIGHT, or BOTTOM) of
    // monitor |monitor| has an auto-hiding taskbar that's always-on-top.
    bool EdgeHasTopmostAutoHideTaskbar(UINT edge, HMONITOR monitor);

    // Adjusts the value of |child_rect| if necessary to ensure that it is
    // completely visible within |parent_rect|.
    void EnsureRectIsVisibleInRect(const gfx::Rect& parent_rect,
        gfx::Rect* child_rect,
        int padding);

    // Ensures that the child window stays within the boundaries of the parent
    // before setting its bounds. If |parent_window| is NULL, the bounds of the
    // parent are assumed to be the bounds of the monitor that |child_window| is
    // nearest to. If |child_window| isn't visible yet and |insert_after_window|
    // is non-NULL and visible, the monitor |insert_after_window| is on is used
    // as the parent bounds instead.
    void SetChildBounds(HWND child_window, HWND parent_window,
        HWND insert_after_window, const gfx::Rect& bounds,
        int padding, unsigned long flags);

    // Returns true if we are on Windows Vista and composition is enabled
    bool ShouldUseVistaFrame();

    // Sets the window mask to a style that most likely matches
    // app/resources/window_*
    void GetDefaultWindowMask(const gfx::Size& size, gfx::Path* window_mask);

} //namespace view

#endif //__windows_util_h__
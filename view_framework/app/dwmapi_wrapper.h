
#ifndef __dwmapi_wrapper_h__
#define __dwmapi_wrapper_h__

#pragma once

#include <windows.h>

namespace view
{

    class DwmapiWrapper
    {
    public:
        // Window attributes
        enum DWMWINDOWATTRIBUTE
        {
            DWMWA_NCRENDERING_ENABLED = 1,      // [get] Is non-client rendering enabled/disabled
            DWMWA_NCRENDERING_POLICY,           // [set] Non-client rendering policy
            DWMWA_TRANSITIONS_FORCEDISABLED,    // [set] Potentially enable/forcibly disable transitions
            DWMWA_ALLOW_NCPAINT,                // [set] Allow contents rendered in the non-client area to be visible on the DWM-drawn frame.
            DWMWA_CAPTION_BUTTON_BOUNDS,        // [get] Bounds of the caption button area in window-relative space.
            DWMWA_NONCLIENT_RTL_LAYOUT,         // [set] Is non-client content RTL mirrored
            DWMWA_FORCE_ICONIC_REPRESENTATION,  // [set] Force this window to display iconic thumbnails.
            DWMWA_FLIP3D_POLICY,                // [set] Designates how Flip3D will treat the window.
            DWMWA_EXTENDED_FRAME_BOUNDS,        // [get] Gets the extended frame bounds rectangle in screen space
            DWMWA_LAST
        };

        // Non-client rendering policy attribute values
        enum DWMNCRENDERINGPOLICY
        {
            DWMNCRP_USEWINDOWSTYLE, // Enable/disable non-client rendering based on window style
            DWMNCRP_DISABLED,       // Disabled non-client rendering; window style is ignored
            DWMNCRP_ENABLED,        // Enabled non-client rendering; window style is ignored
            DWMNCRP_LAST
        };

        typedef HRESULT (WINAPI* DwmIsCompositionEnabledPtr)(__out BOOL* pfEnabled);
        typedef HRESULT (WINAPI* DwmSetWindowAttributePtr)(HWND hwnd,
            DWORD dwAttribute, LPCVOID pvAttribute, DWORD cbAttribute);

        HRESULT DwmIsCompositionEnabled(BOOL* pfEnabled) const;
        HRESULT DwmSetWindowAttribute(HWND hwnd, DWORD dwAttribute,
            LPCVOID pvAttribute, DWORD cbAttribute) const;

        // 获取单件实例.
        static const DwmapiWrapper* instance();

    private:
        DwmapiWrapper();
        ~DwmapiWrapper();

        // dwmapi.dll中的函数指针.
        DwmIsCompositionEnabledPtr dwm_is_composition_enabled_;
        DwmSetWindowAttributePtr dwm_set_window_attribute_;

        // dwmapi.dll模块.
        HMODULE dwmapi_dll_;
    };

} //namespace view

#endif //__dwmapi_wrapper_h__
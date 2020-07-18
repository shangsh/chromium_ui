
#ifndef __rtl_h__
#define __rtl_h__

#include <windows.h>

#include "string16.h"

namespace base
{

    bool IsRTL();
    bool AdjustStringForLocaleDirection(const string16& text,
        string16* localized_text);
    bool StringContainsStrongRTLChars(const string16& text);
    
    // Returns the locale-dependent extended window styles.
    // This function is used for adding locale-dependent extended window styles
    // (e.g. WS_EX_LAYOUTRTL, WS_EX_RTLREADING, etc.) when creating a window.
    // Callers should OR this value into their extended style value when creating
    // a window.
    int GetExtendedStyles();
    
    // TODO(xji):
    // This is a temporary name, it will eventually replace GetExtendedStyles
    int GetExtendedTooltipStyles();
    
    // If the default UI font stored in |logfont| is not suitable, its family
    // and size are replaced with those stored in the per-locale resource.
    void AdjustUIFont(LOGFONT* logfont);

    // If the font for a given window (pointed to by HWND) is not suitable for the
    // UI in the current UI langauge, its family and size are replaced with those
    // stored in the per-locale resource.
    void AdjustUIFontForWindow(HWND hwnd);

} //namespace base

#endif //__rtl_h__
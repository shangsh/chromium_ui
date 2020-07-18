
#include "rtl.h"

namespace base
{

    bool IsRTL()
    {
        // TODO: 根据当前操作系统返回实际值.
        return false;
    }

    bool AdjustStringForLocaleDirection(const string16& text,
        string16* localized_text)
    {
        if(!IsRTL() || text.empty())
        {
            return false;
        }

        // TODO: 根据IsRTL()实际值进行处理.
        *localized_text = text;
        return true;
    }

    bool StringContainsStrongRTLChars(const string16& text)
    {
        // TODO: 根据当前操作系统进行处理.
        return false;
    }

    int GetExtendedStyles() {
        return !IsRTL() ? 0 : WS_EX_LAYOUTRTL | WS_EX_RTLREADING;
    }

    int GetExtendedTooltipStyles() {
        return !IsRTL() ? 0 : WS_EX_LAYOUTRTL;
    }

    void AdjustUIFont(LOGFONT* logfont)
    {
    }

    void AdjustUIFontForWindow(HWND hwnd)
    {
    }

} //namespace base
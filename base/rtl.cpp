
#include "rtl.h"

namespace base
{

    bool IsRTL()
    {
        // TODO: ���ݵ�ǰ����ϵͳ����ʵ��ֵ.
        return false;
    }

    bool AdjustStringForLocaleDirection(const string16& text,
        string16* localized_text)
    {
        if(!IsRTL() || text.empty())
        {
            return false;
        }

        // TODO: ����IsRTL()ʵ��ֵ���д���.
        *localized_text = text;
        return true;
    }

    bool StringContainsStrongRTLChars(const string16& text)
    {
        // TODO: ���ݵ�ǰ����ϵͳ���д���.
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
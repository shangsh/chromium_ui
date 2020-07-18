
#include "dwmapi_wrapper.h"

namespace view
{

    /* static */
    const DwmapiWrapper* DwmapiWrapper::instance()
    {
        // È«¾ÖÊµÀý.
        static const DwmapiWrapper s_dwmapi;
        return &s_dwmapi;
    }

    DwmapiWrapper::DwmapiWrapper()
        : dwmapi_dll_(LoadLibrary(L"dwmapi.dll")),
        dwm_is_composition_enabled_(NULL),
        dwm_set_window_attribute_(NULL)
    {
        if(dwmapi_dll_)
        {
            dwm_is_composition_enabled_ =
                reinterpret_cast<DwmIsCompositionEnabledPtr>(
                GetProcAddress(dwmapi_dll_, "DwmIsCompositionEnabled"));
            dwm_set_window_attribute_ =
                reinterpret_cast<DwmSetWindowAttributePtr>(
                GetProcAddress(dwmapi_dll_, "DwmSetWindowAttribute"));
        }
    }

    DwmapiWrapper::~DwmapiWrapper()
    {
        if(dwmapi_dll_)
        {
            FreeLibrary(dwmapi_dll_);
        }
    }

    HRESULT DwmapiWrapper::DwmIsCompositionEnabled(BOOL* pfEnabled) const
    {
        if(dwm_is_composition_enabled_)
        {
            return dwm_is_composition_enabled_(pfEnabled);
        }

        return E_NOTIMPL;
    }

    HRESULT DwmapiWrapper::DwmSetWindowAttribute(HWND hwnd,
        DWORD dwAttribute, LPCVOID pvAttribute, DWORD cbAttribute) const
    {
        if(dwm_set_window_attribute_)
        {
            return dwm_set_window_attribute_(hwnd, dwAttribute,
                pvAttribute, cbAttribute);
        }

        return E_NOTIMPL;
    }

} //namespace view
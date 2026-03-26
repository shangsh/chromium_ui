#include "gfx/platform_font.h"

#if PLATFORM_WINDOWS
#include "gfx/platform_font_win.h"
#elif PLATFORM_MACOS
#include "gfx/platform_font_mac.h"
#endif

namespace gfx
{

PlatformFont* PlatformFont::CreateDefault()
{
#if PLATFORM_WINDOWS
    return new PlatformFontWin(L"Segoe UI", 12);
#elif PLATFORM_MACOS
    return new PlatformFontMac(L"System", 12);
#else
    return nullptr;
#endif
}

PlatformFont* PlatformFont::CreateFromNameAndSize(const std::wstring& name, int size)
{
#if PLATFORM_WINDOWS
    return new PlatformFontWin(name, size);
#elif PLATFORM_MACOS
    return new PlatformFontMac(name, size);
#else
    return nullptr;
#endif
}

#if PLATFORM_WINDOWS
PlatformFont* PlatformFont::CreateFromNativeFont(HFONT font)
{
    return new PlatformFontWin(font);
}
#endif

} // namespace gfx

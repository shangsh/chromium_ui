#include "gfx/platform_font.h"

#if defined(PLATFORM_WINDOWS)
#include "gfx/platform_font_win.h"
#elif defined(PLATFORM_MACOS)
#include "gfx/platform_font_mac.h"
#endif

namespace gfx
{

PlatformFont* PlatformFont::CreateDefault()
{
#if defined(PLATFORM_WINDOWS)
    return new PlatformFontWin(L"Segoe UI", 12);
#elif defined(PLATFORM_MACOS)
    return new PlatformFontMac(L"System", 12);
#else
    return nullptr;
#endif
}

PlatformFont* PlatformFont::CreateFromNameAndSize(const std::wstring& name, int size)
{
#if defined(PLATFORM_WINDOWS)
    return new PlatformFontWin(name, size);
#elif defined(PLATFORM_MACOS)
    return new PlatformFontMac(name, size);
#else
    return nullptr;
#endif
}

#if defined(PLATFORM_WINDOWS)
PlatformFont* PlatformFont::CreateFromNativeFont(HFONT font)
{
    return new PlatformFontWin(font);
}
#endif

} // namespace gfx

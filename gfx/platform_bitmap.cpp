#include "gfx/platform_bitmap.h"

#if PLATFORM_WINDOWS
#include "gfx/platform_bitmap_win.h"
#elif PLATFORM_MACOS
#include "gfx/platform_bitmap_mac.h"
#endif

namespace gfx
{

PlatformBitmap* PlatformBitmap::CreateDefault()
{
#if PLATFORM_WINDOWS
    return new PlatformBitmapWin(1, 1, true);
#elif PLATFORM_MACOS
    return new PlatformBitmapMac(1, 1, true);
#else
    return nullptr;
#endif
}

PlatformBitmap* PlatformBitmap::CreateFromSize(int width, int height, bool is_opaque)
{
#if PLATFORM_WINDOWS
    return new PlatformBitmapWin(width, height, is_opaque);
#elif PLATFORM_MACOS
    return new PlatformBitmapMac(width, height, is_opaque);
#else
    return nullptr;
#endif
}

#if PLATFORM_WINDOWS
PlatformBitmap* PlatformBitmap::CreateFromNativeBitmap(Gdiplus::Bitmap* bitmap)
{
    return new PlatformBitmapWin(bitmap);
}
#endif

} // namespace gfx

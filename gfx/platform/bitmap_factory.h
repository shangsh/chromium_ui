#ifndef GFX_PLATFORM_BITMAP_H_
#define GFX_PLATFORM_BITMAP_H_

#pragma once

#include "gfx/bitmap.h"

#if defined(PLATFORM_WINDOWS)
#include "gfx/platform/bitmap_win.h"
#elif defined(PLATFORM_MACOS)
#include "gfx/platform/bitmap_mac.h"
#endif

namespace gfx
{

// Platform-specific Bitmap creation
Bitmap* Bitmap::CreateBitmap() {
#if defined(PLATFORM_WINDOWS)
    return new BitmapWin();
#elif defined(PLATFORM_MACOS)
    return new BitmapMac();
#else
    return new Bitmap();
#endif
}

Bitmap* Bitmap::CreateBitmap(int width, int height, bool is_opaque) {
#if defined(PLATFORM_WINDOWS)
    return new BitmapWin(width, height, is_opaque);
#elif defined(PLATFORM_MACOS)
    return new BitmapMac(width, height, is_opaque);
#else
    return new Bitmap();
#endif
}

} // namespace gfx

#endif // GFX_PLATFORM_BITMAP_H_

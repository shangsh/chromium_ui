#ifndef GFX_PLATFORM_BITMAP_H_
#define GFX_PLATFORM_BITMAP_H_

#pragma once

#include "gfx/bitmap.h"
#include "gfx/color.h"
#include <string>

namespace gfx
{

class PlatformBitmap : public base::RefCounted<PlatformBitmap>
{
public:
    static PlatformBitmap* CreateDefault();
    static PlatformBitmap* CreateFromSize(int width, int height, bool is_opaque);

#if defined(PLATFORM_WINDOWS)
    static PlatformBitmap* CreateFromNativeBitmap(Gdiplus::Bitmap* bitmap);
#endif

    virtual int Width() const = 0;
    virtual int Height() const = 0;
    virtual bool IsOpaque() const = 0;
    virtual void* GetBitmapData() const = 0;
    virtual bool LoadFromFile(const std::wstring& file_path) = 0;
    virtual bool SaveToFile(const std::wstring& file_path) const = 0;

#if defined(PLATFORM_WINDOWS)
    virtual Gdiplus::Bitmap* GetNativeBitmap() = 0;
#endif

protected:
    virtual ~PlatformBitmap() {}
public:
    friend class base::RefCounted<PlatformBitmap>;
};

} // namespace gfx

#endif // GFX_PLATFORM_BITMAP_H_

#ifndef GFX_BITMAP_H_
#define GFX_BITMAP_H_

#pragma once

#include "base/ref_counted.h"
#include "color.h"
#include <string>

#if PLATFORM_WINDOWS
namespace Gdiplus
{
    class Bitmap;
}
#endif

namespace gfx
{

class PlatformBitmap;

class Bitmap
{
public:
    Bitmap();
    Bitmap(const Bitmap& other);
    Bitmap& operator=(const Bitmap& other);

#if PLATFORM_WINDOWS
    explicit Bitmap(Gdiplus::Bitmap* native_bitmap);
#endif

    explicit Bitmap(PlatformBitmap* platform_bitmap);
    ~Bitmap();

#if PLATFORM_WINDOWS
    Gdiplus::Bitmap* GetNativeBitmap() const;
#endif

    PlatformBitmap* platform_bitmap() const { return platform_bitmap_.get(); }

    bool IsNull() const;
    int Width() const;
    int Height() const;
    Color GetPixel(int x, int y) const;

    // Platform-independent methods
    int GetWidth() const;
    int GetHeight() const;
    bool IsOpaque() const;
    void* GetBitmapData() const;
    bool LoadFromFile(const std::wstring& file_path);
    bool SaveToFile(const std::wstring& file_path) const;

    // Factory methods
    static Bitmap* CreateBitmap();
    static Bitmap* CreateBitmap(int width, int height, bool is_opaque);

private:
    scoped_refptr<PlatformBitmap> platform_bitmap_;
};

} //namespace gfx

#endif //GFX_BITMAP_H_

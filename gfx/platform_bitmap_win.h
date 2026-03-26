#ifndef GFX_PLATFORM_BITMAP_WIN_H_
#define GFX_PLATFORM_BITMAP_WIN_H_

#pragma once

#include "gfx/platform_bitmap.h"
#ifdef _WIN32
#include <windows.h>
#endif
#include <gdiplus.h>

namespace gfx
{

class PlatformBitmapWin : public PlatformBitmap
{
public:
    PlatformBitmapWin();
    PlatformBitmapWin(int width, int height, bool is_opaque);
    PlatformBitmapWin(Gdiplus::Bitmap* native_bitmap);
    virtual ~PlatformBitmapWin();

    int Width() const override;
    int Height() const override;
    bool IsOpaque() const override;
    void* GetBitmapData() const override;
    bool LoadFromFile(const std::wstring& file_path) override;
    bool SaveToFile(const std::wstring& file_path) const override;

    Gdiplus::Bitmap* GetNativeBitmap() override { return bitmap_; }

private:
    Gdiplus::Bitmap* bitmap_;
};

} // namespace gfx

#endif // GFX_PLATFORM_BITMAP_WIN_H_

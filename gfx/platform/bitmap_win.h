#ifndef GFX_PLATFORM_BITMAP_WIN_H_
#define GFX_PLATFORM_BITMAP_WIN_H_

#pragma once

#include "gfx/bitmap.h"
#include <windows.h>

namespace gfx
{

class BitmapWin : public Bitmap
{
public:
    BitmapWin();
    BitmapWin(int width, int height, bool is_opaque);
    virtual ~BitmapWin();

    // Bitmap interface
    int GetWidth() const override;
    int GetHeight() const override;
    bool IsOpaque() const override;
    void* GetBitmapData() const override;

    // Platform-specific
    HBITMAP GetHBITMAP() const { return bitmap_; }
    HDC GetHDC() const { return memory_dc_; }

    bool LoadFromFile(const std::wstring& file_path) override;
    bool SaveToFile(const std::wstring& file_path) const override;

private:
    void CreateBitmap(int width, int height, bool is_opaque);
    void ReleaseBitmap();

    HBITMAP bitmap_;
    HGDIOBJ old_bitmap_;
    HDC memory_dc_;
    int width_;
    int height_;
    bool is_opaque_;
    std::vector<uint8_t> data_;
};

} // namespace gfx

#endif // GFX_PLATFORM_BITMAP_WIN_H_

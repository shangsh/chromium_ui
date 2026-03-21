#ifndef GFX_PLATFORM_BITMAP_MAC_H_
#define GFX_PLATFORM_BITMAP_MAC_H_

#pragma once

#include "gfx/bitmap.h"
#include <CoreGraphics/CoreGraphics.h>
#include <AppKit/AppKit.h>

namespace gfx
{

class BitmapMac : public Bitmap
{
public:
    BitmapMac();
    BitmapMac(int width, int height, bool is_opaque);
    virtual ~BitmapMac();

    // Bitmap interface
    int GetWidth() const override;
    int GetHeight() const override;
    bool IsOpaque() const override;
    void* GetBitmapData() const override;

    // Platform-specific
    CGImageRef GetCGImage() const { return image_; }
    CGContextRef GetContext() const { return context_; }

    bool LoadFromFile(const std::wstring& file_path) override;
    bool SaveToFile(const std::wstring& file_path) const override;

private:
    void CreateBitmap(int width, int height, bool is_opaque);
    void ReleaseBitmap();

    CGImageRef image_;
    CGContextRef context_;
    int width_;
    int height_;
    bool is_opaque_;
    std::vector<uint8_t> data_;
};

} // namespace gfx

#endif // GFX_PLATFORM_BITMAP_MAC_H_

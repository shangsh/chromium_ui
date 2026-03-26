#ifndef GFX_PLATFORM_BITMAP_MAC_H_
#define GFX_PLATFORM_BITMAP_MAC_H_

#pragma once

#include "gfx/platform_bitmap.h"
#include <CoreGraphics/CoreGraphics.h>
#ifdef __OBJC__
#import <AppKit/AppKit.h>
#endif
#include <vector>

namespace gfx
{

class PlatformBitmapMac : public PlatformBitmap
{
public:
    PlatformBitmapMac();
    PlatformBitmapMac(int width, int height, bool is_opaque);
    virtual ~PlatformBitmapMac();

    int Width() const override { return width_; }
    int Height() const override { return height_; }
    bool IsOpaque() const override { return is_opaque_; }
    void* GetBitmapData() const override;
    bool LoadFromFile(const std::wstring& file_path) override;
    bool SaveToFile(const std::wstring& file_path) const override;

    CGImageRef GetCGImage() const { return image_; }
    CGContextRef GetContext() const { return context_; }

private:
    void CreateBitmap(int width, int height, bool is_opaque);

    CGImageRef image_;
    CGContextRef context_;
    int width_;
    int height_;
    bool is_opaque_;
    std::vector<uint8_t> data_;
};

} // namespace gfx

#endif // GFX_PLATFORM_BITMAP_MAC_H_

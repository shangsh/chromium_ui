#ifndef GFX_PLATFORM_FONT_MAC_H_
#define GFX_PLATFORM_FONT_MAC_H_

#pragma once

#include "gfx/platform_font.h"
#include <CoreText/CoreText.h>
#ifdef __OBJC__
#import <AppKit/AppKit.h>
#endif

namespace gfx
{

class PlatformFontMac : public PlatformFont
{
public:
    PlatformFontMac();
    PlatformFontMac(const std::wstring& font_name, int font_size);
    virtual ~PlatformFontMac();

    std::wstring GetFontName() const override { return font_name_; }
    int GetFontSize() const override { return font_size_; }
    int GetHeight() const override;
    int GetBaseline() const override;
    int GetAverageCharacterWidth() const override;
    int GetStringWidth(const std::wstring& text) const override;
    int GetExpectedTextWidth(int length) const override;
    int GetStyle() const override { return style_; }
    Font DeriveFont(int size_delta, int style) const override;

#if PLATFORM_MACOS && defined(__OBJC__)
    void* GetCTFont() const override { return ct_font_; }
    void* GetNSFont() const override { return ns_font_; }
#endif

    void SetFont(const std::wstring& font_name, int size);

private:
    void CreateFont();
    void ReleaseFont();

    std::wstring font_name_;
    int font_size_;
    int style_;
    CTFontRef ct_font_;
#ifdef __OBJC__
    NSFont* ns_font_;
#else
    void* ns_font_;
#endif
};

} // namespace gfx

#endif // GFX_PLATFORM_FONT_MAC_H_

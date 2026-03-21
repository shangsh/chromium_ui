#ifndef GFX_PLATFORM_FONT_MAC_H_
#define GFX_PLATFORM_FONT_MAC_H_

#pragma once

#include "gfx/font.h"
#include <CoreText/CoreText.h>
#include <AppKit/AppKit.h>

namespace gfx
{

class FontMac : public Font
{
public:
    FontMac();
    FontMac(const std::wstring& font_name, int size);
    virtual ~FontMac();

    // Font interface
    std::wstring GetFontName() const override { return font_name_; }
    int GetFontSize() const override { return font_size_; }
    int GetHeight() const override;
    int GetBaseline() const override;

    // Platform-specific
    CTFontRef GetCTFont() const { return ct_font_; }
    NSFont* GetNSFont() const;

    void SetFont(const std::wstring& font_name, int size);

private:
    void CreateFont();
    void ReleaseFont();

    std::wstring font_name_;
    int font_size_;
    CTFontRef ct_font_;
    NSFont* ns_font_;
};

} // namespace gfx

#endif // GFX_PLATFORM_FONT_MAC_H_

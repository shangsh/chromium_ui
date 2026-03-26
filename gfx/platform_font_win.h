#ifndef GFX_PLATFORM_FONT_WIN_H_
#define GFX_PLATFORM_FONT_WIN_H_

#pragma once

#include "gfx/platform_font.h"
#ifdef _WIN32
#include <windows.h>
#endif

namespace gfx
{

class PlatformFontWin : public PlatformFont
{
public:
    PlatformFontWin();
    PlatformFontWin(const std::wstring& font_name, int font_size);
    PlatformFontWin(HFONT native_font);
    virtual ~PlatformFontWin();

    std::wstring GetFontName() const override { return font_name_; }
    int GetFontSize() const override { return font_size_; }
    int GetHeight() const override;
    int GetBaseline() const override;
    int GetAverageCharacterWidth() const override;
    int GetStringWidth(const std::wstring& text) const override;
    int GetExpectedTextWidth(int length) const override;
    int GetStyle() const override { return style_; }
    Font DeriveFont(int size_delta, int style) const override;

    HFONT GetNativeFont() const override { return font_; }
    void SetFont(const std::wstring& font_name, int size);

private:
    void CreateFont();
    void ReleaseFont();

    std::wstring font_name_;
    int font_size_;
    int style_;
    HFONT font_;
};

} // namespace gfx

#endif // GFX_PLATFORM_FONT_WIN_H_

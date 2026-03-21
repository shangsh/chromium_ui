#ifndef GFX_PLATFORM_FONT_WIN_H_
#define GFX_PLATFORM_FONT_WIN_H_

#pragma once

#include "gfx/font.h"
#include <windows.h>

namespace gfx
{

class FontWin : public Font
{
public:
    FontWin();
    FontWin(const std::wstring& font_name, int size);
    virtual ~FontWin();

    // Font interface
    std::wstring GetFontName() const override { return font_name_; }
    int GetFontSize() const override { return font_size_; }
    int GetHeight() const override;
    int GetBaseline() const override;

    // Platform-specific
    HFONT GetNativeFont() const { return hfont_; }

    void SetFont(const std::wstring& font_name, int size);

private:
    void CreateFont();
    void ReleaseFont();

    std::wstring font_name_;
    int font_size_;
    HFONT hfont_;
};

} // namespace gfx

#endif // GFX_PLATFORM_FONT_WIN_H_

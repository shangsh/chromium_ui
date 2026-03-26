#ifndef GFX_FONT_H_
#define GFX_FONT_H_

#pragma once

#include <string>

#include "base/ref_counted.h"

namespace gfx
{

    class PlatformFont;

    // Font provides platform-independent font wrapper
    class Font
    {
    public:
        enum FontStyle
        {
            NORMAL = 0,
            BOLD = 1,
            ITALIC = 2,
            UNDERLINED = 4,
        };

        Font();
        Font(const Font& other);
        Font& operator=(const Font& other);

#if PLATFORM_WINDOWS
        explicit Font(HFONT native_font);
#endif

        explicit Font(PlatformFont* platform_font);
        Font(const std::wstring& font_name, int font_size);

        ~Font();

        Font DeriveFont(int size_delta) const;
        Font DeriveFont(int size_delta, int style) const;

        int GetHeight() const;
        int GetBaseline() const;
        int GetAverageCharacterWidth() const;
        int GetStringWidth(const std::wstring& text) const;
        int GetExpectedTextWidth(int length) const;
        int GetStyle() const;
        const std::wstring& GetFontName() const;
        int GetFontSize() const;

#if PLATFORM_WINDOWS
        HFONT GetNativeFont() const;
#endif

#if PLATFORM_MACOS && defined(__OBJC__)
        void* GetCTFont() const;
        void* GetNSFont() const;
#endif

        PlatformFont* platform_font() const { return platform_font_.get(); }

    private:
        scoped_refptr<PlatformFont> platform_font_;
    };

} //namespace gfx

#endif //GFX_FONT_H_

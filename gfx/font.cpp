
#include "font.h"

#include "platform_font.h"

namespace gfx
{

    Font::Font() : platform_font_(PlatformFont::CreateDefault()) {}

    Font::Font(const Font& other) : platform_font_(other.platform_font_) {}

    gfx::Font& Font::operator=(const Font& other)
    {
        platform_font_ = other.platform_font_;
        return *this;
    }

    Font::Font(HFONT native_font) : platform_font_(
        PlatformFont::CreateFromNativeFont(native_font)) {}

    Font::Font(PlatformFont* platform_font) : platform_font_(platform_font) {}

    Font::Font(const std::wstring& font_name, int font_size)
        : platform_font_(PlatformFont::CreateFromNameAndSize(font_name,
        font_size)) {}

    Font::~Font() {}

    Font Font::DeriveFont(int size_delta) const
    {
        return DeriveFont(size_delta, GetStyle());
    }

    Font Font::DeriveFont(int size_delta, int style) const
    {
        return platform_font_->DeriveFont(size_delta, style);
    }

    int Font::GetHeight() const
    {
        return platform_font_->GetHeight();
    }

    int Font::GetBaseline() const
    {
        return platform_font_->GetBaseline();
    }

    int Font::GetAverageCharacterWidth() const
    {
        return platform_font_->GetAverageCharacterWidth();
    }

    int Font::GetStringWidth(const std::wstring& text) const
    {
        return platform_font_->GetStringWidth(text);
    }

    int Font::GetExpectedTextWidth(int length) const
    {
        return platform_font_->GetExpectedTextWidth(length);
    }

    int Font::GetStyle() const
    {
        return platform_font_->GetStyle();
    }

    const std::wstring& Font::GetFontName() const
    {
        return platform_font_->GetFontName();
    }

    int Font::GetFontSize() const
    {
        return platform_font_->GetFontSize();
    }

    HFONT Font::GetNativeFont() const
    {
        return platform_font_->GetNativeFont();
    }

} //namespace gfx
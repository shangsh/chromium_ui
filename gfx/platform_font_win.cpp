#include "gfx/platform_font_win.h"
#include "gfx/font.h"

namespace gfx
{

PlatformFontWin::PlatformFontWin()
    : font_name_(L"Segoe UI"),
      font_size_(12),
      style_(Font::NORMAL),
      font_(nullptr)
{
    CreateFont();
}

PlatformFontWin::PlatformFontWin(const std::wstring& font_name, int font_size)
    : font_name_(font_name),
      font_size_(font_size),
      style_(Font::NORMAL),
      font_(nullptr)
{
    CreateFont();
}

PlatformFontWin::PlatformFontWin(HFONT native_font)
    : font_name_(L""),
      font_size_(12),
      style_(Font::NORMAL),
      font_(native_font)
{
    // Get font info from native font
    if (font_)
    {
        LOGFONT lf;
        GetObject(font_, sizeof(lf), &lf);
        font_name_ = lf.lfFaceName;
        font_size_ = -lf.lfHeight;
    }
}

PlatformFontWin::~PlatformFontWin()
{
    ReleaseFont();
}

void PlatformFontWin::CreateFont()
{
    ReleaseFont();

    LOGFONT lf = {};
    lf.lfHeight = -font_size_;
    lf.lfWeight = (style_ & Font::BOLD) ? FW_BOLD : FW_NORMAL;
    lf.lfItalic = (style_ & Font::ITALIC) ? TRUE : FALSE;
    lf.lfUnderline = (style_ & Font::UNDERLINED) ? TRUE : FALSE;
    lf.lfCharSet = DEFAULT_CHARSET;
    lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
    lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
    lf.lfQuality = DEFAULT_QUALITY;
    lf.lfPitchAndFamily = DEFAULT_PITCH | FF_SWISS;
    wcscpy_s(lf.lfFaceName, font_name_.c_str());

    font_ = CreateFontIndirect(&lf);
    if (!font_)
    {
        font_ = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
    }
}

void PlatformFontWin::ReleaseFont()
{
    if (font_)
    {
        DeleteObject(font_);
        font_ = nullptr;
    }
}

int PlatformFontWin::GetHeight() const
{
    if (font_)
    {
        HDC dc = GetDC(nullptr);
        HFONT old_font = (HFONT)SelectObject(dc, font_);
        TEXTMETRIC tm;
        GetTextMetrics(dc, &tm);
        SelectObject(dc, old_font);
        ReleaseDC(nullptr, dc);
        return tm.tmHeight;
    }
    return font_size_;
}

int PlatformFontWin::GetBaseline() const
{
    if (font_)
    {
        HDC dc = GetDC(nullptr);
        HFONT old_font = (HFONT)SelectObject(dc, font_);
        TEXTMETRIC tm;
        GetTextMetrics(dc, &tm);
        SelectObject(dc, old_font);
        ReleaseDC(nullptr, dc);
        return tm.tmAscent;
    }
    return static_cast<int>(font_size_ * 0.8);
}

int PlatformFontWin::GetAverageCharacterWidth() const
{
    if (font_)
    {
        HDC dc = GetDC(nullptr);
        HFONT old_font = (HFONT)SelectObject(dc, font_);
        TEXTMETRIC tm;
        GetTextMetrics(dc, &tm);
        SelectObject(dc, old_font);
        ReleaseDC(nullptr, dc);
        return tm.tmAveCharWidth;
    }
    return font_size_ / 2;
}

int PlatformFontWin::GetStringWidth(const std::wstring& text) const
{
    if (font_ && !text.empty())
    {
        HDC dc = GetDC(nullptr);
        HFONT old_font = (HFONT)SelectObject(dc, font_);
        SIZE size;
        GetTextExtentPoint32(dc, text.c_str(), static_cast<int>(text.length()), &size);
        SelectObject(dc, old_font);
        ReleaseDC(nullptr, dc);
        return size.cx;
    }
    return 0;
}

int PlatformFontWin::GetExpectedTextWidth(int length) const
{
    return GetAverageCharacterWidth() * length;
}

void PlatformFontWin::SetFont(const std::wstring& font_name, int size)
{
    font_name_ = font_name;
    font_size_ = size;
    CreateFont();
}

Font PlatformFontWin::DeriveFont(int size_delta, int style) const
{
    int new_size = font_size_ + size_delta;
    if (new_size <= 0) new_size = 1;
    return Font(new PlatformFontWin(font_name_, new_size));
}

} // namespace gfx

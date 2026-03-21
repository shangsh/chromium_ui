#include "gfx/platform/font_win.h"

namespace gfx
{

FontWin::FontWin()
    : font_name_(L"Segoe UI"),
      font_size_(12),
      hfont_(nullptr)
{
    CreateFont();
}

FontWin::FontWin(const std::wstring& font_name, int size)
    : font_name_(font_name),
      font_size_(size),
      hfont_(nullptr)
{
    CreateFont();
}

FontWin::~FontWin()
{
    ReleaseFont();
}

void FontWin::CreateFont()
{
    ReleaseFont();

    hfont_ = CreateFont(
        font_size_,                    // height
        0,                             // width
        0,                             // escapement
        0,                             // orientation
        FW_NORMAL,                     // weight
        FALSE,                         // italic
        FALSE,                         // underline
        FALSE,                         // strikeout
        DEFAULT_CHARSET,               // charset
        OUT_DEFAULT_PRECIS,           // out precision
        CLIP_DEFAULT_PRECIS,          // clip precision
        DEFAULT_QUALITY,               // quality
        DEFAULT_PITCH | FF_SWISS,     // pitch and family
        font_name_.c_str()            // face name
    );

    if (!hfont_)
    {
        hfont_ = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
    }
}

void FontWin::ReleaseFont()
{
    if (hfont_)
    {
        DeleteObject(hfont_);
        hfont_ = nullptr;
    }
}

int FontWin::GetHeight() const
{
    if (hfont_)
    {
        HDC dc = GetDC(nullptr);
        HFONT old_font = (HFONT)SelectObject(dc, hfont_);
        TEXTMETRIC tm;
        GetTextMetrics(dc, &tm);
        SelectObject(dc, old_font);
        ReleaseDC(nullptr, dc);
        return tm.tmHeight;
    }
    return font_size_;
}

int FontWin::GetBaseline() const
{
    if (hfont_)
    {
        HDC dc = GetDC(nullptr);
        HFONT old_font = (HFONT)SelectObject(dc, hfont_);
        TEXTMETRIC tm;
        GetTextMetrics(dc, &tm);
        SelectObject(dc, old_font);
        ReleaseDC(nullptr, dc);
        return tm.tmAscent;
    }
    return static_cast<int>(font_size_ * 0.8);
}

void FontWin::SetFont(const std::wstring& font_name, int size)
{
    font_name_ = font_name;
    font_size_ = size;
    CreateFont();
}

} // namespace gfx

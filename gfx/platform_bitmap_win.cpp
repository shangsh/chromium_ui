#include "gfx/platform_bitmap_win.h"

namespace gfx
{

PlatformBitmapWin::PlatformBitmapWin()
    : bitmap_(nullptr)
{
}

PlatformBitmapWin::PlatformBitmapWin(int width, int height, bool is_opaque)
    : bitmap_(nullptr)
{
    Gdiplus::PixelFormat format = is_opaque ? PixelFormat32bppARGB : PixelFormat32bppPARGB;
    bitmap_ = new Gdiplus::Bitmap(width, height, format);
}

PlatformBitmapWin::PlatformBitmapWin(Gdiplus::Bitmap* native_bitmap)
    : bitmap_(native_bitmap)
{
}

PlatformBitmapWin::~PlatformBitmapWin()
{
    delete bitmap_;
}

int PlatformBitmapWin::Width() const
{
    if (bitmap_)
    {
        return bitmap_->GetWidth();
    }
    return 0;
}

int PlatformBitmapWin::Height() const
{
    if (bitmap_)
    {
        return bitmap_->GetHeight();
    }
    return 0;
}

bool PlatformBitmapWin::IsOpaque() const
{
    if (bitmap_)
    {
        Gdiplus::PixelFormat format = bitmap_->GetPixelFormat();
        return (format == PixelFormat32bppRGB || format == PixelFormat24bppRGB);
    }
    return true;
}

void* PlatformBitmapWin::GetBitmapData() const
{
    // Would need to lock bitmap bits
    return nullptr;
}

bool PlatformBitmapWin::LoadFromFile(const std::wstring& file_path)
{
    if (bitmap_)
    {
        delete bitmap_;
    }
    bitmap_ = Gdiplus::Bitmap::FromFile(file_path.c_str());
    return bitmap_ && bitmap_->GetLastStatus() == Gdiplus::Ok;
}

bool PlatformBitmapWin::SaveToFile(const std::wstring& file_path) const
{
    if (bitmap_)
    {
        CLSID pngClsid;
        // Get PNG encoder CLSID
        return false;
    }
    return false;
}

} // namespace gfx

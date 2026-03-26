#include "gfx/bitmap.h"
#include "gfx/platform_bitmap.h"

namespace gfx
{

Bitmap::Bitmap() {}

Bitmap::Bitmap(const Bitmap& other) : platform_bitmap_(other.platform_bitmap_) {}

Bitmap& Bitmap::operator=(const Bitmap& other)
{
    platform_bitmap_ = other.platform_bitmap_;
    return *this;
}

#if PLATFORM_WINDOWS
Bitmap::Bitmap(Gdiplus::Bitmap* native_bitmap) : platform_bitmap_(
    PlatformBitmap::CreateFromNativeBitmap(native_bitmap)) {}
#endif

Bitmap::Bitmap(PlatformBitmap* platform_bitmap)
    : platform_bitmap_(platform_bitmap) {}

Bitmap::~Bitmap() {}

#if PLATFORM_WINDOWS
Gdiplus::Bitmap* Bitmap::GetNativeBitmap() const
{
    if(IsNull())
    {
        return NULL;
    }
    return platform_bitmap_->GetNativeBitmap();
}
#endif

bool Bitmap::IsNull() const
{
    return !platform_bitmap_.get();
}

int Bitmap::Width() const
{
    if(!IsNull())
    {
        return platform_bitmap_->Width();
    }
    return 0;
}

int Bitmap::Height() const
{
    if(!IsNull())
    {
        return platform_bitmap_->Height();
    }
    return 0;
}

Color Bitmap::GetPixel(int x, int y) const
{
#if PLATFORM_WINDOWS
    Gdiplus::Color native_color;
    if(!IsNull())
    {
        platform_bitmap_->GetNativeBitmap()->GetPixel(x, y, &native_color);
    }
    Color color;
    color.SetValue(native_color.GetValue());
    return color;
#elif PLATFORM_MACOS
    // Implementation for macOS
    return Color();
#else
    return Color();
#endif
}

int Bitmap::GetWidth() const
{
    return Width();
}

int Bitmap::GetHeight() const
{
    return Height();
}

bool Bitmap::IsOpaque() const
{
    if (!IsNull())
    {
        return platform_bitmap_->IsOpaque();
    }
    return true;
}

void* Bitmap::GetBitmapData() const
{
    if (!IsNull())
    {
        return platform_bitmap_->GetBitmapData();
    }
    return nullptr;
}

bool Bitmap::LoadFromFile(const std::wstring& file_path)
{
    if (!IsNull())
    {
        return platform_bitmap_->LoadFromFile(file_path);
    }
    return false;
}

bool Bitmap::SaveToFile(const std::wstring& file_path) const
{
    if (!IsNull())
    {
        return platform_bitmap_->SaveToFile(file_path);
    }
    return false;
}

} //namespace gfx

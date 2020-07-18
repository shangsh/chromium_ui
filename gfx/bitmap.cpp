
#include "bitmap.h"

#include "platform_bitmap.h"

namespace gfx
{

    Bitmap::Bitmap() {}

    Bitmap::Bitmap(const Bitmap& other) : platform_bitmap_(
        other.platform_bitmap_) {}

    Bitmap& Bitmap::operator=(const Bitmap& other)
    {
        platform_bitmap_ = other.platform_bitmap_;
        return *this;
    }

    Bitmap::Bitmap(Gdiplus::Bitmap* native_bitmap) : platform_bitmap_(
        PlatformBitmap::CreateFromNativeBitmap(native_bitmap)) {}

    Bitmap::Bitmap(PlatformBitmap* platform_bitmap)
        : platform_bitmap_(platform_bitmap) {}

    Bitmap::~Bitmap() {}

    Gdiplus::Bitmap* Bitmap::GetNativeBitmap() const
    {
        if(IsNull())
        {
            return NULL;
        }

        return platform_bitmap_->GetNativeBitmap();
    }

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
        Gdiplus::Color native_color;
        if(!IsNull())
        {
            platform_bitmap_->GetNativeBitmap()->GetPixel(x, y,
                &native_color);
        }

        Color color;
        color.SetValue(native_color.GetValue());
        return color;
    }

} //namespace gfx
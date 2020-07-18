
#include "platform_bitmap_win.h"

#include <algorithm>
using std::min;
using std::max;

#include <windows.h>
#include <gdiplus.h>

#include "base/logging.h"

namespace gfx
{

    PlatformBitmapWin::PlatformBitmapWin(Gdiplus::Bitmap* native_bitmap)
        : bitmap_ref_(new BitmapRef(native_bitmap)) {}

    Gdiplus::Bitmap* PlatformBitmapWin::GetNativeBitmap() const
    {
        return bitmap_ref_->bitmap();
    }

    int PlatformBitmapWin::Width() const
    {
        return bitmap_ref_->bitmap()->GetWidth();
    }

    int PlatformBitmapWin::Height() const
    {
        return bitmap_ref_->bitmap()->GetHeight();
    }


    PlatformBitmapWin::BitmapRef::BitmapRef(Gdiplus::Bitmap* native_bitmap)
        : bitmap_(native_bitmap)
    {
        DLOG_ASSERT(native_bitmap);
    }


    // static
    PlatformBitmap* PlatformBitmap::CreateFromNativeBitmap(
        Gdiplus::Bitmap* native_bitmap)
    {
        return new PlatformBitmapWin(native_bitmap);
    }

} //namespace gfx

#ifndef __platform_bitmap_win_h__
#define __platform_bitmap_win_h__

#pragma once

#include "base/scoped_ptr.h"

#include "platform_bitmap.h"

namespace gfx
{

    class PlatformBitmapWin : public PlatformBitmap
    {
    public:
        explicit PlatformBitmapWin(Gdiplus::Bitmap* native_bitmap);

        virtual Gdiplus::Bitmap* GetNativeBitmap() const;

        virtual int Width() const;

        virtual int Height() const;

    private:
        virtual ~PlatformBitmapWin() {}

        class BitmapRef : public base::RefCounted<BitmapRef>
        {
        public:
            BitmapRef(Gdiplus::Bitmap* native_bitmap);

            Gdiplus::Bitmap* bitmap() const { return bitmap_.get(); }

        private:
            friend class base::RefCounted<BitmapRef>;

            const scoped_ptr<Gdiplus::Bitmap> bitmap_;

            DISALLOW_COPY_AND_ASSIGN(BitmapRef);
        };

        scoped_refptr<BitmapRef> bitmap_ref_;
    };

} //namespace gfx

#endif //__platform_bitmap_win_h__
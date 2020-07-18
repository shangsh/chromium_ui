
#include "bitmap_operations.h"

#include "base/logging.h"

namespace gfx
{

    Bitmap BitmapOperations::CreateDebugBitmap()
    {
        Gdiplus::Bitmap* debug_bitmap = new Gdiplus::Bitmap(32, 32);
        const Gdiplus::Color bright_red(255, 255, 0, 0);
        for(int y=0; y<32; ++y)
        {
            for(int x=0; x<32; ++x)
            {
                debug_bitmap->SetPixel(x, y, bright_red);
            }
        }

        return Bitmap(debug_bitmap);
    }

    // static
    Bitmap BitmapOperations::CreateBlendedBitmap(
        const Bitmap& first_bitmap,
        const Bitmap& second_bitmap,
        double alpha)
    {
        DCHECK(!first_bitmap.IsNull() && !second_bitmap.IsNull());
        DCHECK((alpha>=0) && (alpha<=1));
        Gdiplus::Bitmap* first = first_bitmap.GetNativeBitmap();
        Gdiplus::Bitmap* second = second_bitmap.GetNativeBitmap();
        DCHECK(first->GetWidth() == second->GetWidth());
        DCHECK(first->GetHeight() == second->GetHeight());
        DCHECK(first->GetPixelFormat() == second->GetPixelFormat());
        DCHECK(first->GetPixelFormat() == PixelFormat32bppARGB);

        // 在不需要融合的时候进行优化.
        static const double alpha_min = 1.0 / 255;
        static const double alpha_max = 254.0 / 255;
        if(alpha < alpha_min)
        {
            return first_bitmap;
        }
        else if(alpha > alpha_max)
        {
            return second_bitmap;
        }

        Gdiplus::Bitmap* blended = new Gdiplus::Bitmap(
            first->GetWidth(), first->GetHeight());

        Gdiplus::BitmapData lock_first;
        Gdiplus::BitmapData lock_second;
        Gdiplus::BitmapData lock_blended;
        Gdiplus::Rect rc_lock(0, 0, first->GetWidth(), first->GetHeight());
        first->LockBits(&rc_lock, Gdiplus::ImageLockModeRead,
            PixelFormat32bppARGB, &lock_first);
        second->LockBits(&rc_lock, Gdiplus::ImageLockModeRead,
            PixelFormat32bppARGB, &lock_second);
        blended->LockBits(&rc_lock, Gdiplus::ImageLockModeWrite,
            PixelFormat32bppARGB, &lock_blended);

        double first_alpha = 1 - alpha;

        for(UINT y=0; y<lock_first.Height; ++y)
        {
            uint32* first_row = static_cast<uint32*>(lock_first.Scan0) +
                y*lock_first.Width;
            uint32* second_row = static_cast<uint32*>(lock_second.Scan0) +
                y*lock_second.Width;
            uint32* dst_row = static_cast<uint32*>(lock_blended.Scan0) +
                y*lock_blended.Width;

            for(UINT x=0; x<lock_first.Width; ++x)
            {
                Gdiplus::Color first_pixel(first_row[x]);
                Gdiplus::Color second_pixel(second_row[x]);

                int a = static_cast<int>((first_pixel.GetA()*first_alpha) +
                    (second_pixel.GetA()*alpha));
                int r = static_cast<int>((first_pixel.GetR()*first_alpha) +
                    (second_pixel.GetR()*alpha));
                int g = static_cast<int>((first_pixel.GetG()*first_alpha) +
                    (second_pixel.GetG()*alpha));
                int b = static_cast<int>((first_pixel.GetB()*first_alpha) +
                    (second_pixel.GetB()*alpha));

                dst_row[x] = Gdiplus::Color::MakeARGB(a, r, g, b);
            }
        }

        blended->UnlockBits(&lock_blended);
        second->UnlockBits(&lock_second);
        first->UnlockBits(&lock_first);

        return Bitmap(blended);
    }

    // static
    Bitmap BitmapOperations::CreateButtonBackground(const Color& color,
        const Bitmap& image_bitmap,
        const Bitmap& mask_bitmap)
    {
        DCHECK(!image_bitmap.IsNull() && !mask_bitmap.IsNull());
        Gdiplus::Bitmap* image = image_bitmap.GetNativeBitmap();
        Gdiplus::Bitmap* mask = mask_bitmap.GetNativeBitmap();
        DCHECK(image->GetPixelFormat() == PixelFormat32bppARGB);
        DCHECK(mask->GetPixelFormat() == PixelFormat32bppARGB);

        Gdiplus::Bitmap* background = new Gdiplus::Bitmap(
            mask->GetWidth(), mask->GetHeight());
        double bg_a = color.GetA();
        double bg_r = color.GetR();
        double bg_g = color.GetG();
        double bg_b = color.GetB();

        Gdiplus::BitmapData lock_mask;
        Gdiplus::BitmapData lock_background;
        Gdiplus::BitmapData lock_image;
        {
            Gdiplus::Rect rc_lock(0, 0, mask->GetWidth(), mask->GetHeight());
            mask->LockBits(&rc_lock, Gdiplus::ImageLockModeRead,
                PixelFormat32bppARGB, &lock_mask);
            background->LockBits(&rc_lock, Gdiplus::ImageLockModeWrite,
                PixelFormat32bppARGB, &lock_background);
        }
        {
            Gdiplus::Rect rc_lock(0, 0, image->GetWidth(), image->GetHeight());
            image->LockBits(&rc_lock, Gdiplus::ImageLockModeRead,
                PixelFormat32bppARGB, &lock_image);
        }

        for(UINT y=0; y<lock_mask.Height; ++y)
        {
            uint32* dst_row = static_cast<uint32*>(lock_background.Scan0) +
                y*lock_background.Width;
            uint32* image_row = static_cast<uint32*>(lock_image.Scan0) +
                (y%lock_image.Height)*lock_image.Width;
            uint32* mask_row = static_cast<uint32*>(lock_mask.Scan0) +
                y*lock_mask.Width;

            for(UINT x=0; x<lock_mask.Width; ++x)
            {
                uint32 image_pixel = image_row[x%lock_image.Width];
                Gdiplus::Color clr_image_pixel(image_pixel);
                Gdiplus::Color clr_mask_pixel(mask_row[x]);

                double img_a = clr_image_pixel.GetA();
                double img_r = clr_image_pixel.GetR();
                double img_g = clr_image_pixel.GetG();
                double img_b = clr_image_pixel.GetB();

                double img_alpha = static_cast<double>(img_a) / 255.0;
                double img_inv = 1 - img_alpha;

                double mask_a = static_cast<double>(clr_mask_pixel.GetA()) / 255.0;

                dst_row[x] = Gdiplus::Color::MakeARGB(
                    static_cast<int>(__min(255.0, bg_a+img_a)*mask_a),
                    static_cast<int>(((bg_r*img_inv)+(img_r*img_alpha))*mask_a),
                    static_cast<int>(((bg_g*img_inv)+(img_g*img_alpha))*mask_a),
                    static_cast<int>(((bg_b*img_inv)+(img_b*img_alpha))*mask_a));
            }
        }

        image->UnlockBits(&lock_image);
        background->UnlockBits(&lock_background);
        mask->UnlockBits(&lock_mask);

        return Bitmap(background);
    }

} //namespace gfx

#include "color_utils.h"

#include <math.h>

#include "base/logging.h"

namespace gfx
{

    namespace
    {

        double calcHue(double temp1, double temp2, double hue)
        {
            if(hue < 0.0)
            {
                ++hue;
            }
            else if(hue > 1.0)
            {
                --hue;
            }

            if(hue*6.0 < 1.0)
            {
                return temp1 + (temp2-temp1)*hue*6.0;
            }
            if(hue*2.0 < 1.0)
            {
                return temp2;
            }
            if(hue*3.0 < 2.0)
            {
                return temp1 + (temp2-temp1)*(2.0/3.0-hue)*6.0;
            }

            return temp1;
        }

        int GetLumaForColor(const Color& color)
        {
            int luma = static_cast<int>((0.3*color.GetR()) +
                (0.59*color.GetG()) + (0.11*color.GetB()));
            return __max(__min(luma, 255), 0);
        }

        // 下面两个函数的公式来自于:
        // http://www.w3.org/TR/WCAG20/#relativeluminancedef
        // http://www.w3.org/TR/WCAG20/#contrast-ratiodef

        double ConvertSRGB(double eight_bit_component)
        {
            const double component = eight_bit_component / 255.0;
            return (component<=0.03928) ? (component/12.92) :
                pow((component+0.055)/1.055, 2.4);
        }

        Color LumaInvertColor(const Color& color)
        {
            HSL hsl;
            ColorToHSL(color, &hsl);
            hsl.l = 1.0 - hsl.l;
            return HSLToColor(hsl, 255);
        }

        double ContrastRatio(double foreground_luminance, double background_luminance)
        {
            // 注意: 传递的值必须来自RelativeLuminance(), 因为那样可以保证值>0, 不会
            // 发生除0的错误.
            return (foreground_luminance>background_luminance) ?
                (foreground_luminance/background_luminance) :
            (background_luminance / foreground_luminance);
        }

    }

    double RelativeLuminance(const Color& color)
    {
        return (0.2126*ConvertSRGB(color.GetR())) +
            (0.7152*ConvertSRGB(color.GetG())) +
            (0.0722*ConvertSRGB(color.GetB())) + 0.05;
    }

    void ColorToHSL(const Color& color, HSL* hsl)
    {
        double r = static_cast<double>(color.GetR()) / 255.0;
        double g = static_cast<double>(color.GetG()) / 255.0;
        double b = static_cast<double>(color.GetB()) / 255.0;
        double vmax = __max(__max(r, g), b);
        double vmin = __min(__min(r, g), b);
        double delta = vmax - vmin;
        hsl->l = (vmax + vmin) / 2;
        if(delta)
        {
            double dr = (((vmax - r) / 6.0) + (delta / 2.0)) / delta;
            double dg = (((vmax - g) / 6.0) + (delta / 2.0)) / delta;
            double db = (((vmax - b) / 6.0) + (delta / 2.0)) / delta;
            // We need to compare for the max value because comparing vmax to r,
            // g or b can sometimes result in values overflowing registers.
            if(r>=g && r>=b)
            {
                hsl->h = db - dg;
            }
            else if(g>=r && g>=b)
            {
                hsl->h = (1.0 / 3.0) + dr - db;
            }
            else // (b>=r && b>=g)
            {
                hsl->h = (2.0 / 3.0) + dg - dr;
            }

            if(hsl->h < 0.0)
            {
                ++hsl->h;
            }
            else if(hsl->h > 1.0)
            {
                --hsl->h;
            }

            hsl->s = delta / ((hsl->l < 0.5) ? (vmax + vmin) : (2 - vmax - vmin));
        }
        else
        {
            hsl->h = hsl->s = 0;
        }
    }

    Color HSLToColor(const HSL& hsl, uint8 alpha)
    {
        double hue = hsl.h;
        double saturation = hsl.s;
        double lightness = hsl.l;

        // If there's no color, we don't care about hue and can do everything based
        // on brightness.
        if(!saturation)
        {
            uint8 light;

            if(lightness < 0)
            {
                light = 0;
            }
            else if(lightness >= 1.0)
            {
                light = 255;
            }
            else
            {
                light = static_cast<int32>(lightness) >> 8;
            }

            return Color(alpha, light, light, light);
        }

        double temp2 = (lightness<0.5) ? (lightness*(1.0+saturation))
            : (lightness+saturation-(lightness*saturation));
        double temp1 = 2.0 * lightness - temp2;
        return Color(alpha,
            static_cast<uint8>(calcHue(temp1, temp2, hue+1.0/3.0) * 255),
            static_cast<uint8>(calcHue(temp1, temp2, hue) * 255),
            static_cast<uint8>(calcHue(temp1, temp2, hue-1.0/3.0) * 255));
    }

    Color HSLShift(const Color& color, const HSL& shift)
    {
        HSL hsl;
        int alpha = color.GetA();
        ColorToHSL(color, &hsl);

        // Replace the hue with the tint's hue.
        if(shift.h >= 0)
        {
            hsl.h = shift.h;
        }

        // Change the saturation.
        if(shift.s >= 0)
        {
            if(shift.s <= 0.5)
            {
                hsl.s *= shift.s * 2.0;
            }
            else
            {
                hsl.s += (1.0 - hsl.s) * ((shift.s - 0.5) * 2.0);
            }
        }

        Color result = HSLToColor(hsl, alpha);

        if(shift.l < 0)
        {
            return result;
        }

        // Lightness shifts in the style of popular image editors aren't
        // actually represented in HSL - the L value does have some effect
        // on saturation.
        double r = static_cast<double>(result.GetR());
        double g = static_cast<double>(result.GetG());
        double b = static_cast<double>(result.GetB());
        if(shift.l <= 0.5)
        {
            r *= (shift.l * 2.0);
            g *= (shift.l * 2.0);
            b *= (shift.l * 2.0);
        }
        else
        {
            r += (255.0 - r) * ((shift.l - 0.5) * 2.0);
            g += (255.0 - g) * ((shift.l - 0.5) * 2.0);
            b += (255.0 - b) * ((shift.l - 0.5) * 2.0);
        }
        return Color(alpha, static_cast<uint8>(r), static_cast<uint8>(g),
            static_cast<uint8>(b));
    }

    bool IsColorCloseToTransparent(uint8 alpha)
    {
        const int kCloseToBoundary = 64;
        return alpha<kCloseToBoundary;
    }

    bool IsColorCloseToGrey(int r, int g, int b)
    {
        const int kAverageBoundary = 15;
        int average = (r + g + b) / 3;
        return (abs(r-average)<kAverageBoundary) &&
            (abs(g-average)<kAverageBoundary) &&
            (abs(b-average)<kAverageBoundary);
    }

    Color GetAverageColorOfFavicon(const Bitmap& favicon_bitmap, uint8 alpha)
    {
        DCHECK(!favicon_bitmap.IsNull());

        int r=0, g=0, b=0;

        Gdiplus::Bitmap* favicon = favicon_bitmap.GetNativeBitmap();
        Gdiplus::BitmapData favicon_lock;
        Gdiplus::Rect rc_lock(0, 0, favicon->GetWidth(), favicon->GetHeight());
        favicon->LockBits(&rc_lock, Gdiplus::ImageLockModeRead,
            PixelFormat32bppARGB, &favicon_lock);
        uint32* pixels = static_cast<uint32*>(favicon_lock.Scan0);
        // 假定为ARGB_8888格式.
        DCHECK(favicon->GetPixelFormat() == PixelFormat32bppARGB);
        uint32* current = pixels;

        DCHECK(favicon_lock.Width<=16 && favicon_lock.Height<=16);

        int pixel_count = favicon_lock.Width * favicon_lock.Height;
        int color_count = 0;
        for(int i=0; i<pixel_count; ++i,++current)
        {
            // 丢弃接近黑色、白色或者透明色的颜色, 因为这些像素对平均值影响不大.
            Color current_color(*current);
            int cr = current_color.GetR();
            int cg = current_color.GetG();
            int cb = current_color.GetB();

            if(IsColorCloseToTransparent(current_color.GetA()) ||
                IsColorCloseToGrey(cr, cg, cb))
            {
                continue;
            }

            r += cr;
            g += cg;
            b += cb;
            ++color_count;
        }
        favicon->UnlockBits(&favicon_lock);

        return color_count ?
            Color(alpha, r/color_count, g/color_count, b/color_count) :
        Color(alpha, 0, 0, 0);
    }

    void BuildLumaHistogram(const Bitmap& bitmap, int histogram[256])
    {
        DCHECK(!bitmap.IsNull());

        Gdiplus::Bitmap* native_bitmap = bitmap.GetNativeBitmap();
        Gdiplus::BitmapData bitmap_lock;
        Gdiplus::Rect rc_lock(0, 0, native_bitmap->GetWidth(),
            native_bitmap->GetHeight());
        native_bitmap->LockBits(&rc_lock, Gdiplus::ImageLockModeRead,
            PixelFormat32bppARGB, &bitmap_lock);
        uint32* pixels = static_cast<uint32*>(bitmap_lock.Scan0);
        // 假定为ARGB_8888格式.
        DCHECK(native_bitmap->GetPixelFormat() == PixelFormat32bppARGB);

        int pixel_width = static_cast<int>(bitmap_lock.Width);
        int pixel_height = static_cast<int>(bitmap_lock.Height);
        for(int y=0; y<pixel_height; ++y)
        {
            uint32* current = pixels + y*pixel_width;
            for(int x=0; x<pixel_width; ++x,++current)
            {
                histogram[GetLumaForColor(Color(*current))]++;
            }
        }

        native_bitmap->UnlockBits(&bitmap_lock);
    }

    Color AlphaBlend(const Color& foreground, const Color& background,
        uint8 alpha)
    {
        if(alpha == 0)
        {
            return background;
        }
        if(alpha == 255)
        {
            return foreground;
        }
        uint32 alpha_u32 = static_cast<uint32>(alpha);
        return Color(
            ((foreground.GetR()*alpha_u32)+
            (background.GetR()*(255-alpha_u32)))/255,
            ((foreground.GetG()*alpha_u32)+
            (background.GetG()*(255-alpha_u32)))/255,
            ((foreground.GetB()*alpha_u32)+
            (background.GetB()*(255-alpha_u32)))/255);
    }

    Color GetReadableColor(const Color& foreground, const Color& background)
    {
        const Color foreground2 = LumaInvertColor(foreground);
        const double background_luminance = RelativeLuminance(background);
        return (ContrastRatio(RelativeLuminance(foreground), background_luminance) >=
            ContrastRatio(RelativeLuminance(foreground2), background_luminance))
            ? foreground : foreground2;
    }

    Color GetSysColor(int which)
    {
        return Color(::GetSysColor(which));
    }

} //namespace gfx
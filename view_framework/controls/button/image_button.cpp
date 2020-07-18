
#include "image_button.h"

#include "animation/throb_animation.h"

#include "gfx/canvas.h"
#include "gfx/bitmap_operations.h"

namespace view
{

    static const int kDefaultWidth = 16;  // Default button width if no theme.
    static const int kDefaultHeight = 14; // Default button height if no theme.

    ////////////////////////////////////////////////////////////////////////////////
    // ImageButton, public:

    ImageButton::ImageButton(ButtonListener* listener)
        : CustomButton(listener),
        h_alignment_(ALIGN_LEFT),
        v_alignment_(ALIGN_TOP),
        preferred_size_(kDefaultWidth, kDefaultHeight)
    {
        // By default, we request that the gfx::Canvas passed to our View::Paint()
        // implementation is flipped horizontally so that the button's bitmaps are
        // mirrored when the UI directionality is right-to-left.
        EnableCanvasFlippingForRTLUI(true);
    }

    ImageButton::~ImageButton() {}

    void ImageButton::SetImage(ButtonState aState, const gfx::Bitmap& anImage)
    {
        images_[aState] = anImage;
        PreferredSizeChanged();
    }

    void ImageButton::SetBackground(const gfx::Color& color,
        const gfx::Bitmap& image,
        const gfx::Bitmap& mask)
    {
        if(image.IsNull() || mask.IsNull())
        {
            background_image_ = gfx::Bitmap(
                gfx::BitmapOperations::CreateDebugBitmap());
            return;
        }

        background_image_ = gfx::BitmapOperations::CreateButtonBackground(
            color, image, mask);
    }

    void ImageButton::SetImageAlignment(HorizontalAlignment h_align,
        VerticalAlignment v_align)
    {
        h_alignment_ = h_align;
        v_alignment_ = v_align;
        SchedulePaint();
    }

    ////////////////////////////////////////////////////////////////////////////////
    // ImageButton, View overrides:

    gfx::Size ImageButton::GetPreferredSize()
    {
        if(!images_[BS_NORMAL].IsNull())
        {
            return gfx::Size(images_[BS_NORMAL].Width(),
                images_[BS_NORMAL].Height());
        }
        return preferred_size_;
    }

    void ImageButton::Paint(gfx::Canvas* canvas)
    {
        // Call the base class first to paint any background/borders.
        View::Paint(canvas);

        gfx::Bitmap img = GetImageToPaint();

        if(!img.IsNull())
        {
            int x = 0, y = 0;

            if(h_alignment_ == ALIGN_CENTER)
            {
                x = (width() - img.Width()) / 2;
            }
            else if(h_alignment_ == ALIGN_RIGHT)
            {
                x = width() - img.Width();
            }

            if(v_alignment_ == ALIGN_MIDDLE)
            {
                y = (height() - img.Height()) / 2;
            }
            else if (v_alignment_ == ALIGN_BOTTOM)
            {
                y = height() - img.Height();
            }

            if(!background_image_.IsNull())
            {
                canvas->DrawBitmapInt(background_image_, x, y);
            }
            canvas->DrawBitmapInt(img, x, y);
        }
        PaintFocusBorder(canvas);
    }

    ////////////////////////////////////////////////////////////////////////////////
    // ImageButton, protected:

    gfx::Bitmap ImageButton::GetImageToPaint()
    {
        gfx::Bitmap img;

        if (!images_[BS_HOT].IsNull() && hover_animation_->is_animating())
        {
            img = gfx::BitmapOperations::CreateBlendedBitmap(images_[BS_NORMAL],
                images_[BS_HOT], hover_animation_->GetCurrentValue());
        }
        else
        {
            img = images_[state_];
        }

        return !img.IsNull() ? img : images_[BS_NORMAL];
    }

    ////////////////////////////////////////////////////////////////////////////////
    // ToggleImageButton, public:

    ToggleImageButton::ToggleImageButton(ButtonListener* listener)
        : ImageButton(listener), toggled_(false) {}

    ToggleImageButton::~ToggleImageButton() {}

    void ToggleImageButton::SetToggled(bool toggled)
    {
        if(toggled == toggled_)
        {
            return;
        }

        for(int i=0; i<BS_COUNT; ++i)
        {
            gfx::Bitmap temp = images_[i];
            images_[i] = alternate_images_[i];
            alternate_images_[i] = temp;
        }
        toggled_ = toggled;
        SchedulePaint();
    }

    void ToggleImageButton::SetToggledImage(ButtonState state,
        const gfx::Bitmap image)
    {
        if(toggled_)
        {
            images_[state] = image;
            if(state_ == state)
            {
                SchedulePaint();
            }
        }
        else
        {
            alternate_images_[state] = image;
        }
    }

    void ToggleImageButton::SetToggledTooltipText(const std::wstring& tooltip)
    {
        toggled_tooltip_text_.assign(tooltip);
    }

    ////////////////////////////////////////////////////////////////////////////////
    // ToggleImageButton, ImageButton overrides:

    void ToggleImageButton::SetImage(ButtonState state, const gfx::Bitmap image)
    {
        if(toggled_)
        {
            alternate_images_[state] = image;
        }
        else
        {
            images_[state] = image;
            if(state_ == state)
            {
                SchedulePaint();
            }
        }
        PreferredSizeChanged();
    }

    ////////////////////////////////////////////////////////////////////////////////
    // ToggleImageButton, View overrides:

    bool ToggleImageButton::GetTooltipText(const gfx::Point& p,
        std::wstring* tooltip)
    {
        if(!toggled_ || toggled_tooltip_text_.empty())
        {
            return Button::GetTooltipText(p, tooltip);
        }

        *tooltip = toggled_tooltip_text_;
        return true;
    }

} //namespace view
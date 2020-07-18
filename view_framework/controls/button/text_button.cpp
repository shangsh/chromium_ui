
#include "text_button.h"

#include <algorithm>

#include "animation/throb_animation.h"

#include "gfx/canvas_gdiplus.h"

#include "../../app/event.h"
#include "../../app/resource_bundle.h"
#include "button.h"

namespace view
{

    // Padding between the icon and text.
    static const int kIconTextPadding = 5;

    // Preferred padding between text and edge
    static const int kPreferredPaddingHorizontal = 6;
    static const int kPreferredPaddingVertical = 5;

    // static
    const gfx::Color TextButton::kEnabledColor = gfx::Color(6, 45, 117);
    // static
    const gfx::Color TextButton::kHighlightColor = gfx::Color(200, 255, 255, 255);
    // static
    const gfx::Color TextButton::kDisabledColor = gfx::Color(161, 161, 146);
    // static
    const gfx::Color TextButton::kHoverColor = TextButton::kEnabledColor;

    // How long the hover fade animation should last.
    static const int kHoverAnimationDurationMs = 300;

    // static
    const char TextButton::kViewClassName[] = "view/TextButton";

    static int PrefixTypeToCanvasType(TextButton::PrefixType type)
    {
        switch(type)
        {
        case TextButton::PREFIX_HIDE:
            return gfx::Canvas::HIDE_PREFIX;
        case TextButton::PREFIX_SHOW:
            return gfx::Canvas::SHOW_PREFIX;
        case TextButton::PREFIX_NONE:
            return 0;
        default:
            NOTREACHED();
            return 0;
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    //
    // TextButtonBorder - constructors, destructors, initialization
    //
    ////////////////////////////////////////////////////////////////////////////////

    TextButtonBorder::TextButtonBorder()
    {
        ResourceBundle& rb = ResourceBundle::GetSharedInstance();

        hot_set_.top_left = rb.GetBitmapAppNamed(
            ResourceBundle::BITMAP_TEXTBUTTON_TOP_LEFT_H);
        hot_set_.top = rb.GetBitmapAppNamed(
            ResourceBundle::BITMAP_TEXTBUTTON_TOP_H);
        hot_set_.top_right = rb.GetBitmapAppNamed(
            ResourceBundle::BITMAP_TEXTBUTTON_TOP_RIGHT_H);
        hot_set_.left = rb.GetBitmapAppNamed(
            ResourceBundle::BITMAP_TEXTBUTTON_LEFT_H);
        hot_set_.center = rb.GetBitmapAppNamed(
            ResourceBundle::BITMAP_TEXTBUTTON_CENTER_H);
        hot_set_.right = rb.GetBitmapAppNamed(
            ResourceBundle::BITMAP_TEXTBUTTON_RIGHT_H);
        hot_set_.bottom_left = rb.GetBitmapAppNamed(
            ResourceBundle::BITMAP_TEXTBUTTON_BOTTOM_LEFT_H);
        hot_set_.bottom = rb.GetBitmapAppNamed(
            ResourceBundle::BITMAP_TEXTBUTTON_BOTTOM_H);
        hot_set_.bottom_right = rb.GetBitmapAppNamed(
            ResourceBundle::BITMAP_TEXTBUTTON_BOTTOM_RIGHT_H);

        pushed_set_.top_left = rb.GetBitmapAppNamed(
            ResourceBundle::BITMAP_TEXTBUTTON_TOP_LEFT_P);
        pushed_set_.top = rb.GetBitmapAppNamed(
            ResourceBundle::BITMAP_TEXTBUTTON_TOP_P);
        pushed_set_.top_right = rb.GetBitmapAppNamed(
            ResourceBundle::BITMAP_TEXTBUTTON_TOP_RIGHT_P);
        pushed_set_.left = rb.GetBitmapAppNamed(
            ResourceBundle::BITMAP_TEXTBUTTON_LEFT_P);
        pushed_set_.center = rb.GetBitmapAppNamed(
            ResourceBundle::BITMAP_TEXTBUTTON_CENTER_P);
        pushed_set_.right = rb.GetBitmapAppNamed(
            ResourceBundle::BITMAP_TEXTBUTTON_RIGHT_P);
        pushed_set_.bottom_left = rb.GetBitmapAppNamed(
            ResourceBundle::BITMAP_TEXTBUTTON_BOTTOM_LEFT_P);
        pushed_set_.bottom = rb.GetBitmapAppNamed(
            ResourceBundle::BITMAP_TEXTBUTTON_BOTTOM_P);
        pushed_set_.bottom_right = rb.GetBitmapAppNamed(
            ResourceBundle::BITMAP_TEXTBUTTON_BOTTOM_RIGHT_P);
    }

    TextButtonBorder::~TextButtonBorder() {}


    ////////////////////////////////////////////////////////////////////////////////
    //
    // TextButtonBorder - painting
    //
    ////////////////////////////////////////////////////////////////////////////////

    void TextButtonBorder::Paint(const View& view, gfx::Canvas* canvas) const
    {
        const TextButton* mb = static_cast<const TextButton*>(&view);
        int state = mb->state();

        // TextButton takes care of deciding when to call Paint.
        const MBBImageSet* set = &hot_set_;
        if(state == TextButton::BS_PUSHED)
        {
            set = &pushed_set_;
        }

        if(set)
        {
            Paint(view, canvas, *set);
        }
        else
        {
            // Do nothing
        }
    }

    void TextButtonBorder::Paint(const View& view, gfx::Canvas* canvas,
        const MBBImageSet& set) const
    {
        gfx::Rect bounds = view.bounds();

        // Draw the top left image
        canvas->DrawBitmapInt(set.top_left, 0, 0);

        // Tile the top image
        canvas->TileImageInt(
            set.top,
            set.top_left.Width(),
            0,
            bounds.width()-set.top_right.Width()-set.top_left.Width(),
            set.top.Height());

        // Draw the top right image
        canvas->DrawBitmapInt(set.top_right,
            bounds.width()-set.top_right.Width(), 0);

        // Tile the left image
        canvas->TileImageInt(
            set.left,
            0,
            set.top_left.Height(),
            set.top_left.Width(),
            bounds.height()-set.top.Height()-set.bottom_left.Height());

        // Tile the center image
        canvas->TileImageInt(
            set.center,
            set.left.Width(),
            set.top.Height(),
            bounds.width()-set.right.Width()-set.left.Width(),
            bounds.height()-set.bottom.Height()-set.top.Height());

        // Tile the right image
        canvas->TileImageInt(
            set.right,
            bounds.width()-set.right.Width(),
            set.top_right.Height(),
            set.right.Width(),
            bounds.height()-set.bottom_right.Height()-set.top_right.Height());

        // Draw the bottom left image
        canvas->DrawBitmapInt(set.bottom_left,
            0, bounds.height()-set.bottom_left.Height());

        // Tile the bottom image
        canvas->TileImageInt(
            set.bottom,
            set.bottom_left.Width(),
            bounds.height()-set.bottom.Height(),
            bounds.width()-set.bottom_right.Width()-set.bottom_left.Width(),
            set.bottom.Height());

        // Draw the bottom right image
        canvas->DrawBitmapInt(set.bottom_right,
            bounds.width()-set.bottom_right.Width(),
            bounds.height()-set.bottom_right.Height());
    }

    void TextButtonBorder::GetInsets(gfx::Insets* insets) const
    {
        insets->Set(kPreferredPaddingVertical, kPreferredPaddingHorizontal,
            kPreferredPaddingVertical, kPreferredPaddingHorizontal);
    }

    ////////////////////////////////////////////////////////////////////////////////
    // TextButton, public:

    TextButton::TextButton(ButtonListener* listener, const std::wstring& text)
        : CustomButton(listener),
        alignment_(ALIGN_LEFT),
        icon_placement_(ICON_ON_LEFT),
        font_(ResourceBundle::GetSharedInstance().GetFont(
        ResourceBundle::BaseFont)),
        color_(kEnabledColor),
        color_enabled_(kEnabledColor),
        color_disabled_(kDisabledColor),
        color_highlight_(kHighlightColor),
        color_hover_(kHoverColor),
        has_hover_icon_(false),
        has_pushed_icon_(false),
        max_width_(0),
        normal_has_border_(false),
        show_multiple_icon_states_(true),
        prefix_type_(PREFIX_NONE)
    {
        SetText(text);
        set_border(new TextButtonBorder);
        SetAnimationDuration(kHoverAnimationDurationMs);
    }

    TextButton::~TextButton() {}

    void TextButton::SetText(const std::wstring& text)
    {
        text_ = text;
        SetAccessibleName(text);
        UpdateTextSize();
    }

    void TextButton::SetIcon(const gfx::Bitmap& icon)
    {
        icon_ = icon;
    }

    void TextButton::SetHoverIcon(const gfx::Bitmap& icon)
    {
        icon_hover_ = icon;
        has_hover_icon_ = true;
    }

    void TextButton::SetPushedIcon(const gfx::Bitmap& icon)
    {
        icon_pushed_ = icon;
        has_pushed_icon_ = true;
    }

    void TextButton::SetFont(const gfx::Font& font)
    {
        font_ = font;
        UpdateTextSize();
    }

    void TextButton::SetEnabledColor(const gfx::Color& color)
    {
        color_enabled_ = color;
        UpdateColor();
    }

    void TextButton::SetDisabledColor(const gfx::Color& color)
    {
        color_disabled_ = color;
        UpdateColor();
    }

    void TextButton::SetHighlightColor(const gfx::Color& color)
    {
        color_highlight_ = color;
    }

    void TextButton::SetHoverColor(const gfx::Color& color)
    {
        color_hover_ = color;
    }

    void TextButton::ClearMaxTextSize()
    {
        max_text_size_ = text_size_;
    }

    void TextButton::SetNormalHasBorder(bool normal_has_border)
    {
        normal_has_border_ = normal_has_border;
    }

    void TextButton::SetShowMultipleIconStates(bool show_multiple_icon_states)
    {
        show_multiple_icon_states_ = show_multiple_icon_states;
    }

    void TextButton::Paint(gfx::Canvas* canvas, bool for_drag)
    {
        if(!for_drag)
        {
            PaintBackground(canvas);

            if(show_multiple_icon_states_ && hover_animation_->is_animating())
            {
                // Draw the hover bitmap into an offscreen buffer, then blend it
                // back into the current canvas.
                canvas->SaveLayer(
                    static_cast<int>(hover_animation_->GetCurrentValue()*255));
                canvas->AsCanvasGdiplus()->Clear(gfx::Color(0, 255, 255, 255));
                PaintBorder(canvas);
                canvas->RestoreLayer();
            }
            else if((show_multiple_icon_states_ &&
                (state_==BS_HOT || state_==BS_PUSHED)) ||
                (state_==BS_NORMAL && normal_has_border_))
            {
                PaintBorder(canvas);
            }

            PaintFocusBorder(canvas);
        }

        gfx::Bitmap icon = icon_;
        if(show_multiple_icon_states_)
        {
            if(has_hover_icon_ && (state()==BS_HOT))
            {
                icon = icon_hover_;
            }
            else if(has_pushed_icon_ && (state()==BS_PUSHED))
            {
                icon = icon_pushed_;
            }
        }

        gfx::Insets insets = GetInsets();
        int available_width = width() - insets.width();
        int available_height = height() - insets.height();
        // Use the actual text (not max) size to properly center the text.
        int content_width = text_size_.width();
        if(icon.Width() > 0)
        {
            content_width += icon.Width();
            if(!text_.empty())
            {
                content_width += kIconTextPadding;
            }
        }
        // Place the icon along the left edge.
        int icon_x;
        if(alignment_ == ALIGN_LEFT)
        {
            icon_x = insets.left();
        }
        else if (alignment_ == ALIGN_RIGHT)
        {
            icon_x = available_width - content_width;
        }
        else
        {
            icon_x = __max(0, (available_width-content_width)/2) + insets.left();
        }
        int text_x = icon_x;
        if(icon.Width() > 0)
        {
            text_x += icon.Width() + kIconTextPadding;
        }
        const int text_width = __min(text_size_.width(),
            width()-insets.right()-text_x);
        int text_y = (available_height-text_size_.height())/2 + insets.top();

        // If the icon should go on the other side, swap the elements.
        if(icon_placement_ == ICON_ON_RIGHT)
        {
            int new_text_x = icon_x;
            icon_x = new_text_x + text_width + kIconTextPadding;
            text_x = new_text_x;
        }

        if(text_width > 0)
        {
            // Because the text button can (at times) draw multiple elements on the
            // canvas, we can not mirror the button by simply flipping the canvas as
            // doing this will mirror the text itself. Flipping the canvas will also
            // make the icons look wrong because icons are almost always represented as
            // direction insentisive bitmaps and such bitmaps should never be flipped
            // horizontally.
            //
            // Due to the above, we must perform the flipping manually for RTL UIs.
            gfx::Rect text_bounds(text_x, text_y, text_width, text_size_.height());
            text_bounds.set_x(MirroredLeftPointForRect(text_bounds));

            gfx::Color text_color = (show_multiple_icon_states_ &&
                (state()==BS_HOT || state()==BS_PUSHED)) ? color_hover_ : color_;

            int draw_string_flags = gfx::CanvasGdiplus::DefaultCanvasTextAlignment() |
                PrefixTypeToCanvasType(prefix_type_);

            if(for_drag)
            {
                // TODO(erg): Either port DrawStringWithHalo to linux or find an
                // alternative here.
                canvas->AsCanvasGdiplus()->DrawStringWithHalo(
                    text_, font_, text_color, color_highlight_, text_bounds.x(),
                    text_bounds.y(), text_bounds.width(), text_bounds.height(),
                    draw_string_flags);
            }
            else
            {
                canvas->DrawStringInt(text_,
                    font_,
                    text_color,
                    text_bounds.x(),
                    text_bounds.y(),
                    text_bounds.width(),
                    text_bounds.height(),
                    draw_string_flags);
            }
        }

        if(icon.Width() > 0)
        {
            int icon_y = (available_height-icon.Height())/2 + insets.top();

            // Mirroring the icon position if necessary.
            gfx::Rect icon_bounds(icon_x, icon_y, icon.Width(), icon.Height());
            icon_bounds.set_x(MirroredLeftPointForRect(icon_bounds));
            canvas->DrawBitmapInt(icon, icon_bounds.x(), icon_bounds.y());
        }
    }

    void TextButton::UpdateColor()
    {
        color_ = IsEnabled() ? color_enabled_ : color_disabled_;
    }

    void TextButton::UpdateTextSize()
    {
        int width = 0, height = 0;
        gfx::CanvasGdiplus::SizeStringInt(
            text_, font_, &width, &height,
            gfx::Canvas::NO_ELLIPSIS|PrefixTypeToCanvasType(prefix_type_));
        text_size_.SetSize(width, font_.GetHeight());
        max_text_size_.SetSize(__max(max_text_size_.width(), text_size_.width()),
            __max(max_text_size_.height(),
            text_size_.height()));
        PreferredSizeChanged();
    }

    ////////////////////////////////////////////////////////////////////////////////
    // TextButton, View overrides:

    gfx::Size TextButton::GetPreferredSize()
    {
        gfx::Insets insets = GetInsets();

        // Use the max size to set the button boundaries.
        gfx::Size prefsize(max_text_size_.width()+icon_.Width()+insets.width(),
            __max(max_text_size_.height(), icon_.Height())+insets.height());

        if(icon_.Width()>0 && !text_.empty())
        {
            prefsize.Enlarge(kIconTextPadding, 0);
        }

        if(max_width_ > 0)
        {
            prefsize.set_width(__min(max_width_, prefsize.width()));
        }

        return prefsize;
    }

    gfx::Size TextButton::GetMinimumSize()
    {
        return max_text_size_;
    }

    void TextButton::SetEnabled(bool enabled)
    {
        if(enabled != IsEnabled())
        {
            CustomButton::SetEnabled(enabled);
        }
        // We should always call UpdateColor() since the state of the button might be
        // changed by other functions like CustomButton::SetState().
        UpdateColor();
        SchedulePaint();
    }

    std::string TextButton::GetClassName() const
    {
        return kViewClassName;
    }

    void TextButton::Paint(gfx::Canvas* canvas)
    {
        Paint(canvas, false);
    }

} //namespace view
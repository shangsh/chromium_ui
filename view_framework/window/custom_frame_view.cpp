
#include "custom_frame_view.h"

#include "base/win_util.h"

#include "gfx/canvas.h"
#include "gfx/font.h"
#include "gfx/path.h"

#include "../app/resource_bundle.h"
#include "../app/windows_util.h"
#include "client_view.h"
#include "window_delegate.h"

namespace view
{

    // static
    gfx::Font* CustomFrameView::title_font_ = NULL;

    namespace
    {
        // The frame border is only visible in restored mode and is hardcoded to 4 px on
        // each side regardless of the system window border size.
        const int kFrameBorderThickness = 4;
        // Various edges of the frame border have a 1 px shadow along their edges; in a
        // few cases we shift elements based on this amount for visual appeal.
        const int kFrameShadowThickness = 1;
        // While resize areas on Windows are normally the same size as the window
        // borders, our top area is shrunk by 1 px to make it easier to move the window
        // around with our thinner top grabbable strip.  (Incidentally, our side and
        // bottom resize areas don't match the frame border thickness either -- they
        // span the whole nonclient area, so there's no "dead zone" for the mouse.)
        const int kTopResizeAdjust = 1;
        // In the window corners, the resize areas don't actually expand bigger, but the
        // 16 px at the end of each edge triggers diagonal resizing.
        const int kResizeAreaCornerSize = 16;
        // The titlebar never shrinks too short to show the caption button plus some
        // padding below it.
        const int kCaptionButtonHeightWithPadding = 19;
        // The titlebar has a 2 px 3D edge along the top and bottom.
        const int kTitlebarTopAndBottomEdgeThickness = 2;
        // The icon is inset 2 px from the left frame border.
        const int kIconLeftSpacing = 2;
        // The icon never shrinks below 16 px on a side.
        const int kIconMinimumSize = 16;
        // There is a 4 px gap between the icon and the title text.
        const int kIconTitleSpacing = 4;
        // There is a 5 px gap between the title text and the caption buttons.
        const int kTitleCaptionSpacing = 5;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // CustomFrameView, public:

    CustomFrameView::CustomFrameView(Window* frame)
        : NonClientFrameView(),
        close_button_(new ImageButton(this)),
        restore_button_(new ImageButton(this)),
        maximize_button_(new ImageButton(this)),
        minimize_button_(new ImageButton(this)),
        window_icon_(NULL),
        should_show_minmax_buttons_(false),
        should_show_client_edge_(false),
        frame_(frame)
    {
        InitClass();

        ResourceBundle& rb = ResourceBundle::GetSharedInstance();

        // WLW TODO: fix it.
        //close_button_->SetAccessibleName(l10n_util::GetString(IDS_APP_ACCNAME_CLOSE));

        // Close button images will be set in LayoutWindowControls().
        AddChildView(close_button_);

        // WLW TODO: fix it.
        //restore_button_->SetAccessibleName(
        //    l10n_util::GetString(IDS_APP_ACCNAME_RESTORE));
        restore_button_->SetImage(CustomButton::BS_NORMAL,
            rb.GetBitmapAppNamed(ResourceBundle::BITMAP_RESTORE));
        restore_button_->SetImage(CustomButton::BS_HOT,
            rb.GetBitmapAppNamed(ResourceBundle::BITMAP_RESTORE_H));
        restore_button_->SetImage(CustomButton::BS_PUSHED,
            rb.GetBitmapAppNamed(ResourceBundle::BITMAP_RESTORE_P));
        AddChildView(restore_button_);

        // WLW TODO: fix it.
        //maximize_button_->SetAccessibleName(
        //  l10n_util::GetString(IDS_APP_ACCNAME_MAXIMIZE));
        maximize_button_->SetImage(CustomButton::BS_NORMAL,
            rb.GetBitmapAppNamed(ResourceBundle::BITMAP_MAXIMIZE));
        maximize_button_->SetImage(CustomButton::BS_HOT,
            rb.GetBitmapAppNamed(ResourceBundle::BITMAP_MAXIMIZE_H));
        maximize_button_->SetImage(CustomButton::BS_PUSHED,
            rb.GetBitmapAppNamed(ResourceBundle::BITMAP_MAXIMIZE_P));
        AddChildView(maximize_button_);

        // WLW TODO: fix it.
        //minimize_button_->SetAccessibleName(
        //    l10n_util::GetString(IDS_APP_ACCNAME_MINIMIZE));
        minimize_button_->SetImage(CustomButton::BS_NORMAL,
            rb.GetBitmapAppNamed(ResourceBundle::BITMAP_MINIMIZE));
        minimize_button_->SetImage(CustomButton::BS_HOT,
            rb.GetBitmapAppNamed(ResourceBundle::BITMAP_MINIMIZE_H));
        minimize_button_->SetImage(CustomButton::BS_PUSHED,
            rb.GetBitmapAppNamed(ResourceBundle::BITMAP_MINIMIZE_P));
        AddChildView(minimize_button_);

        view::WindowDelegate* d = frame_->GetDelegate();
        should_show_minmax_buttons_ = d->CanMaximize();
        should_show_client_edge_ = d->ShouldShowClientEdge();

        if(d->ShouldShowWindowIcon())
        {
            window_icon_ = new ImageButton(this);
            AddChildView(window_icon_);
        }
    }

    CustomFrameView::~CustomFrameView() {}

    ///////////////////////////////////////////////////////////////////////////////
    // CustomFrameView, NonClientFrameView implementation:

    gfx::Rect CustomFrameView::GetBoundsForClientView() const
    {
        return client_view_bounds_;
    }

    gfx::Rect CustomFrameView::GetWindowBoundsForClientBounds(
        const gfx::Rect& client_bounds) const
    {
        int top_height = NonClientTopBorderHeight();
        int border_thickness = NonClientBorderThickness();
        return gfx::Rect(__max(0, client_bounds.x() - border_thickness),
            __max(0, client_bounds.y() - top_height),
            client_bounds.width() + (2 * border_thickness),
            client_bounds.height() + top_height + border_thickness);
    }

    int CustomFrameView::NonClientHitTest(const gfx::Point& point)
    {
        // Sanity check.
        if(!bounds().Contains(point))
        {
            return HTNOWHERE;
        }

        int frame_component = frame_->GetClientView()->NonClientHitTest(point);

        // See if we're in the sysmenu region.  (We check the ClientView first to be
        // consistent with OpaqueBrowserFrameView; it's not really necessary here.)
        gfx::Rect sysmenu_rect(IconBounds());
        // In maximized mode we extend the rect to the screen corner to take advantage
        // of Fitts' Law.
        if(frame_->IsMaximized())
        {
            sysmenu_rect.SetRect(0, 0, sysmenu_rect.right(), sysmenu_rect.bottom());
        }
        sysmenu_rect.set_x(MirroredLeftPointForRect(sysmenu_rect));
        if(sysmenu_rect.Contains(point))
        {
            return (frame_component == HTCLIENT) ? HTCLIENT : HTSYSMENU;
        }

        if(frame_component != HTNOWHERE)
        {
            return frame_component;
        }

        // Then see if the point is within any of the window controls.
        if(close_button_->GetBounds(APPLY_MIRRORING_TRANSFORMATION).Contains(point))
        {
            return HTCLOSE;
        }
        if(restore_button_->GetBounds(APPLY_MIRRORING_TRANSFORMATION).Contains(
            point))
        {
            return HTMAXBUTTON;
        }
        if(maximize_button_->GetBounds(APPLY_MIRRORING_TRANSFORMATION).Contains(
            point))
        {
            return HTMAXBUTTON;
        }
        if(minimize_button_->GetBounds(APPLY_MIRRORING_TRANSFORMATION).Contains(
            point))
        {
            return HTMINBUTTON;
        }
        if(window_icon_ &&
            window_icon_->GetBounds(APPLY_MIRRORING_TRANSFORMATION).Contains(point))
        {
            return HTSYSMENU;
        }

        int window_component = GetHTComponentForFrame(point, FrameBorderThickness(),
            NonClientBorderThickness(), kResizeAreaCornerSize, kResizeAreaCornerSize,
            frame_->GetDelegate()->CanResize());
        // Fall back to the caption if no other component matches.
        return (window_component == HTNOWHERE) ? HTCAPTION : window_component;
    }

    void CustomFrameView::GetWindowMask(const gfx::Size& size, gfx::Path* window_mask)
    {
        DCHECK(window_mask);
        if(frame_->IsMaximized())
        {
            return;
        }

        view::GetDefaultWindowMask(size, window_mask);
    }

    void CustomFrameView::EnableClose(bool enable)
    {
        close_button_->SetEnabled(enable);
    }

    void CustomFrameView::ResetWindowControls()
    {
        restore_button_->SetState(CustomButton::BS_NORMAL);
        minimize_button_->SetState(CustomButton::BS_NORMAL);
        maximize_button_->SetState(CustomButton::BS_NORMAL);
        // The close button isn't affected by this constraint.
    }

    ///////////////////////////////////////////////////////////////////////////////
    // CustomFrameView, View overrides:

    void CustomFrameView::Paint(gfx::Canvas* canvas)
    {
        if(frame_->IsMaximized())
        {
            PaintMaximizedFrameBorder(canvas);
        }
        else
        {
            PaintRestoredFrameBorder(canvas);
        }
        PaintTitleBar(canvas);
        if(ShouldShowClientEdge())
        {
            PaintRestoredClientEdge(canvas);
        }
    }

    void CustomFrameView::Layout()
    {
        LayoutWindowControls();
        LayoutTitleBar();
        LayoutClientView();
    }

    gfx::Size CustomFrameView::GetPreferredSize()
    {
        gfx::Size pref = frame_->GetClientView()->GetPreferredSize();
        gfx::Rect bounds(0, 0, pref.width(), pref.height());
        return frame_->GetNonClientView()->GetWindowBoundsForClientBounds(
            bounds).size();
    }

    ///////////////////////////////////////////////////////////////////////////////
    // CustomFrameView, ButtonListener implementation:

    void CustomFrameView::ButtonPressed(Button* sender, const Event& event)
    {
        if(sender == close_button_)
        {
            frame_->Close();
        }
        else if(sender == minimize_button_)
        {
            frame_->Minimize();
        }
        else if(sender == maximize_button_)
        {
            frame_->Maximize();
        }
        else if(sender == restore_button_)
        {
            frame_->Restore();
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    // CustomFrameView, private:

    int CustomFrameView::FrameBorderThickness() const
    {
        return frame_->IsMaximized() ? 0 : kFrameBorderThickness;
    }

    int CustomFrameView::NonClientBorderThickness() const
    {
        // In maximized mode, we don't show a client edge.
        return FrameBorderThickness() +
            (ShouldShowClientEdge() ? kClientEdgeThickness : 0);
    }

    int CustomFrameView::NonClientTopBorderHeight() const
    {
        return __max(FrameBorderThickness() + IconSize(),
            CaptionButtonY() + kCaptionButtonHeightWithPadding) +
            TitlebarBottomThickness();
    }

    int CustomFrameView::CaptionButtonY() const
    {
        // Maximized buttons start at window top so that even if their images aren't
        // drawn flush with the screen edge, they still obey Fitts' Law.
        return frame_->IsMaximized() ? FrameBorderThickness() : kFrameShadowThickness;
    }

    int CustomFrameView::TitlebarBottomThickness() const
    {
        return kTitlebarTopAndBottomEdgeThickness +
            (ShouldShowClientEdge() ? kClientEdgeThickness : 0);
    }

    int CustomFrameView::IconSize() const
    {
        // This metric scales up if either the titlebar height or the titlebar font
        // size are increased.
        return GetSystemMetrics(SM_CYSMICON);
    }

    bool CustomFrameView::ShouldShowClientEdge() const
    {
        return should_show_client_edge_ && !frame_->IsMaximized();
    }

    gfx::Rect CustomFrameView::IconBounds() const
    {
        int size = IconSize();
        int frame_thickness = FrameBorderThickness();
        // Our frame border has a different "3D look" than Windows'.  Theirs has a
        // more complex gradient on the top that they push their icon/title below;
        // then the maximized window cuts this off and the icon/title are centered
        // in the remaining space.  Because the apparent shape of our border is
        // simpler, using the same positioning makes things look slightly uncentered
        // with restored windows, so when the window is restored, instead of
        // calculating the remaining space from below the frame border, we calculate
        // from below the 3D edge.
        int unavailable_px_at_top = frame_->IsMaximized() ? frame_thickness :
            kTitlebarTopAndBottomEdgeThickness;
        // When the icon is shorter than the minimum space we reserve for the caption
        // button, we vertically center it.  We want to bias rounding to put extra
        // space above the icon, since the 3D edge (+ client edge, for restored
        // windows) below looks (to the eye) more like additional space than does the
        // 3D edge (or nothing at all, for maximized windows) above; hence the +1.
        int y = unavailable_px_at_top + (NonClientTopBorderHeight() -
            unavailable_px_at_top - size - TitlebarBottomThickness() + 1) / 2;
        return gfx::Rect(frame_thickness + kIconLeftSpacing, y, size, size);
    }

    void CustomFrameView::PaintRestoredFrameBorder(gfx::Canvas* canvas)
    {
        // Window frame mode.
        ResourceBundle& rb = ResourceBundle::GetSharedInstance();

        gfx::Bitmap frame_image;
        gfx::Color frame_color;
        if(frame_->IsActive())
        {
          frame_image = rb.GetBitmapAppNamed(ResourceBundle::BITMAP_FRAME);
          frame_color = ResourceBundle::frame_color;
        }
        else
        {
          frame_image = rb.GetBitmapAppNamed(ResourceBundle::BITMAP_FRAME_INACTIVE);
          frame_color = ResourceBundle::frame_color_inactive;
        }

        const gfx::Bitmap& top_left_corner = rb.GetBitmapAppNamed(
            ResourceBundle::BITMAP_WINDOW_TOP_LEFT_CORNER);
        gfx::Bitmap top_right_corner =
            rb.GetBitmapAppNamed(ResourceBundle::BITMAP_WINDOW_TOP_RIGHT_CORNER);
        const gfx::Bitmap& top_edge = rb.GetBitmapAppNamed(
            ResourceBundle::BITMAP_WINDOW_TOP_CENTER);
        const gfx::Bitmap& right_edge = rb.GetBitmapAppNamed(
            ResourceBundle::BITMAP_WINDOW_RIGHT_SIDE);
        const gfx::Bitmap& left_edge = rb.GetBitmapAppNamed(
            ResourceBundle::BITMAP_WINDOW_LEFT_SIDE);
        const gfx::Bitmap& bottom_left_corner =
            rb.GetBitmapAppNamed(ResourceBundle::BITMAP_WINDOW_BOTTOM_LEFT_CORNER);
        const gfx::Bitmap& bottom_right_corner =
            rb.GetBitmapAppNamed(ResourceBundle::BITMAP_WINDOW_BOTTOM_RIGHT_CORNER);
        const gfx::Bitmap& bottom_edge = rb.GetBitmapAppNamed(
            ResourceBundle::BITMAP_WINDOW_BOTTOM_CENTER);

        // Fill with the frame color first so we have a constant background for
        // areas not covered by the theme image.
        canvas->FillRectInt(frame_color, 0, 0, width(), frame_image.Height());
        // Now fill down the sides.
        canvas->FillRectInt(frame_color, 0, frame_image.Height(), left_edge.Width(),
            height()-frame_image.Height());
        canvas->FillRectInt(frame_color, width()-right_edge.Width(),
            frame_image.Height(), right_edge.Width(),
            height()-frame_image.Height());
        // Now fill the bottom area.
        canvas->FillRectInt(frame_color,
            left_edge.Width(), height()-bottom_edge.Height(),
            width()-left_edge.Width()-right_edge.Width(), bottom_edge.Height());

        // Draw the theme frame.
        canvas->TileImageInt(frame_image, 0, 0, width(), frame_image.Height());

        // Top.
        canvas->DrawBitmapInt(top_left_corner, 0, 0);
        canvas->TileImageInt(top_edge, top_left_corner.Width(), 0,
            width()-top_right_corner.Width(), top_edge.Height());
        canvas->DrawBitmapInt(top_right_corner, width()-top_right_corner.Width(), 0);

        // Right.
        canvas->TileImageInt(right_edge, width()-right_edge.Width(),
            top_right_corner.Height(), right_edge.Width(),
            height()-top_right_corner.Height()-bottom_right_corner.Height());

        // Bottom.
        canvas->DrawBitmapInt(bottom_right_corner,
            width()-bottom_right_corner.Width(),
            height()-bottom_right_corner.Height());
        canvas->TileImageInt(bottom_edge, bottom_left_corner.Width(),
            height()-bottom_edge.Height(),
            width()-bottom_left_corner.Width()-bottom_right_corner.Width(),
            bottom_edge.Height());
        canvas->DrawBitmapInt(bottom_left_corner, 0,
            height()-bottom_left_corner.Height());

        // Left.
        canvas->TileImageInt(left_edge, 0, top_left_corner.Height(),
            left_edge.Width(),
            height()-top_left_corner.Height()-bottom_left_corner.Height());
    }

    void CustomFrameView::PaintMaximizedFrameBorder(gfx::Canvas* canvas)
    {
        ResourceBundle& rb = ResourceBundle::GetSharedInstance();

        const gfx::Bitmap& frame_image = rb.GetBitmapAppNamed(frame_->IsActive() ?
            ResourceBundle::BITMAP_FRAME : ResourceBundle::BITMAP_FRAME_INACTIVE);
        canvas->TileImageInt(frame_image, 0, FrameBorderThickness(), width(),
            frame_image.Height());

        // The bottom of the titlebar actually comes from the top of the Client Edge
        // graphic, with the actual client edge clipped off the bottom.
        const gfx::Bitmap& titlebar_bottom = rb.GetBitmapAppNamed(
            ResourceBundle::BITMAP_APP_TOP_CENTER);
        int edge_height = titlebar_bottom.Height() -
            ShouldShowClientEdge() ? kClientEdgeThickness : 0;
        canvas->TileImageInt(titlebar_bottom, 0,
            frame_->GetClientView()->y()-edge_height, width(), edge_height);
    }

    void CustomFrameView::PaintTitleBar(gfx::Canvas* canvas)
    {
        WindowDelegate* d = frame_->GetDelegate();

        // It seems like in some conditions we can be asked to paint after the window
        // that contains us is WM_DESTROYed. At this point, our delegate is NULL. The
        // correct long term fix may be to shut down the RootView in WM_DESTROY.
        if(!d)
        {
            return;
        }

        canvas->DrawStringInt(d->GetWindowTitle(), *title_font_, gfx::ColorWHITE,
            MirroredLeftPointForRect(title_bounds_), title_bounds_.y(),
            title_bounds_.width(), title_bounds_.height());
    }

    void CustomFrameView::PaintRestoredClientEdge(gfx::Canvas* canvas)
    {
        gfx::Rect client_area_bounds = frame_->GetClientView()->bounds();
        int client_area_top = client_area_bounds.y();

        ResourceBundle& rb = ResourceBundle::GetSharedInstance();
        const gfx::Bitmap& top_left = rb.GetBitmapAppNamed(
            ResourceBundle::BITMAP_APP_TOP_LEFT);
        const gfx::Bitmap& top = rb.GetBitmapAppNamed(
            ResourceBundle::BITMAP_APP_TOP_CENTER);
        const gfx::Bitmap& top_right = rb.GetBitmapAppNamed(
            ResourceBundle::BITMAP_APP_TOP_RIGHT);
        const gfx::Bitmap& right = rb.GetBitmapAppNamed(
            ResourceBundle::BITMAP_CONTENT_RIGHT_SIDE);
        const gfx::Bitmap& bottom_right =
            rb.GetBitmapAppNamed(ResourceBundle::BITMAP_CONTENT_BOTTOM_RIGHT_CORNER);
        const gfx::Bitmap& bottom = rb.GetBitmapAppNamed(
            ResourceBundle::BITMAP_CONTENT_BOTTOM_CENTER);
        const gfx::Bitmap& bottom_left =
            rb.GetBitmapAppNamed(ResourceBundle::BITMAP_CONTENT_BOTTOM_LEFT_CORNER);
        const gfx::Bitmap& left = rb.GetBitmapAppNamed(
            ResourceBundle::BITMAP_CONTENT_LEFT_SIDE);

        // Top.
        int top_edge_y = client_area_top - top.Height();
        canvas->DrawBitmapInt(top_left, client_area_bounds.x()-top_left.Width(),
            top_edge_y);
        canvas->TileImageInt(top, client_area_bounds.x(), top_edge_y,
            client_area_bounds.width(), top.Height());
        canvas->DrawBitmapInt(top_right, client_area_bounds.right(), top_edge_y);

        // Right.
        int client_area_bottom =
            __max(client_area_top, client_area_bounds.bottom());
        int client_area_height = client_area_bottom - client_area_top;
        canvas->TileImageInt(right, client_area_bounds.right(), client_area_top,
            right.Width(), client_area_height);

        // Bottom.
        canvas->DrawBitmapInt(bottom_right, client_area_bounds.right(),
            client_area_bottom);
        canvas->TileImageInt(bottom, client_area_bounds.x(), client_area_bottom,
            client_area_bounds.width(), bottom_right.Height());
        canvas->DrawBitmapInt(bottom_left,
            client_area_bounds.x()-bottom_left.Width(), client_area_bottom);

        // Left.
        canvas->TileImageInt(left, client_area_bounds.x()-left.Width(),
            client_area_top, left.Width(), client_area_height);

        // Draw the toolbar color to fill in the edges.
        canvas->DrawRectInt(ResourceBundle::toolbar_color,
            client_area_bounds.x()-1, client_area_top-1,
            client_area_bounds.width()+1, client_area_bottom-client_area_top+1);
    }

    void CustomFrameView::LayoutWindowControls()
    {
        close_button_->SetImageAlignment(ImageButton::ALIGN_LEFT,
            ImageButton::ALIGN_BOTTOM);
        int caption_y = CaptionButtonY();
        bool is_maximized = frame_->IsMaximized();
        // There should always be the same number of non-shadow pixels visible to the
        // side of the caption buttons.  In maximized mode we extend the rightmost
        // button to the screen corner to obey Fitts' Law.
        int right_extra_width = is_maximized ?
            (kFrameBorderThickness - kFrameShadowThickness) : 0;
        gfx::Size close_button_size = close_button_->GetPreferredSize();
        close_button_->SetBounds(width() - FrameBorderThickness() -
            right_extra_width - close_button_size.width(), caption_y,
            close_button_size.width() + right_extra_width,
            close_button_size.height());

        // When the window is restored, we show a maximized button; otherwise, we show
        // a restore button.
        bool is_restored = !is_maximized && !frame_->IsMinimized();
        view::ImageButton* invisible_button = is_restored ? restore_button_ :
            maximize_button_;
        invisible_button->SetVisible(false);

        view::ImageButton* visible_button = is_restored ? maximize_button_ :
            restore_button_;
        ResourceBundle::AppId normal_part, hot_part, pushed_part;
        if(should_show_minmax_buttons_)
        {
            visible_button->SetVisible(true);
            visible_button->SetImageAlignment(ImageButton::ALIGN_LEFT,
                ImageButton::ALIGN_BOTTOM);
            gfx::Size visible_button_size = visible_button->GetPreferredSize();
            visible_button->SetBounds(close_button_->x() - visible_button_size.width(),
                caption_y, visible_button_size.width(),
                visible_button_size.height());

            minimize_button_->SetVisible(true);
            minimize_button_->SetImageAlignment(ImageButton::ALIGN_LEFT,
                ImageButton::ALIGN_BOTTOM);
            gfx::Size minimize_button_size = minimize_button_->GetPreferredSize();
            minimize_button_->SetBounds(
                visible_button->x() - minimize_button_size.width(), caption_y,
                minimize_button_size.width(),
                minimize_button_size.height());

            normal_part = ResourceBundle::BITMAP_CLOSE;
            hot_part = ResourceBundle::BITMAP_CLOSE_H;
            pushed_part = ResourceBundle::BITMAP_CLOSE_P;
        }
        else
        {
            visible_button->SetVisible(false);
            minimize_button_->SetVisible(false);

            normal_part = ResourceBundle::BITMAP_CLOSE_SA;
            hot_part = ResourceBundle::BITMAP_CLOSE_SA_H;
            pushed_part = ResourceBundle::BITMAP_CLOSE_SA_P;
        }

        ResourceBundle& rb = ResourceBundle::GetSharedInstance();

        close_button_->SetImage(CustomButton::BS_NORMAL, rb.GetBitmapAppNamed(normal_part));
        close_button_->SetImage(CustomButton::BS_HOT, rb.GetBitmapAppNamed(hot_part));
        close_button_->SetImage(CustomButton::BS_PUSHED, rb.GetBitmapAppNamed(pushed_part));
    }

    void CustomFrameView::LayoutTitleBar()
    {
        // The window title is based on the calculated icon position, even when there
        // is no icon.
        gfx::Rect icon_bounds(IconBounds());
        view::WindowDelegate* d = frame_->GetDelegate();
        if(d->ShouldShowWindowIcon())
        {
            window_icon_->SetBounds(icon_bounds);
        }

        // Size the title.
        int title_x = d->ShouldShowWindowIcon() ?
            icon_bounds.right()+kIconTitleSpacing : icon_bounds.x();
        int title_height = title_font_->GetHeight();
        // We bias the title position so that when the difference between the icon and
        // title heights is odd, the extra pixel of the title is above the vertical
        // midline rather than below.  This compensates for how the icon is already
        // biased downwards (see IconBounds()) and helps prevent descenders on the
        // title from overlapping the 3D edge at the bottom of the titlebar.
        title_bounds_.SetRect(title_x,
            icon_bounds.y()+((icon_bounds.height()-title_height-1)/2),
            __max(0,
            (should_show_minmax_buttons_?minimize_button_->x():close_button_->x())-
            kTitleCaptionSpacing-title_x), title_height);
    }

    void CustomFrameView::LayoutClientView()
    {
        int top_height = NonClientTopBorderHeight();
        int border_thickness = NonClientBorderThickness();
        client_view_bounds_.SetRect(border_thickness, top_height,
            __max(0, width()-(2*border_thickness)),
            __max(0, height()-top_height-border_thickness));
    }

    // static
    void CustomFrameView::InitClass()
    {
        static bool initialized = false;
        if(!initialized)
        {
            title_font_ = new gfx::Font(GetWindowTitleFont());
            initialized = true;
        }
    }

} //namespace view
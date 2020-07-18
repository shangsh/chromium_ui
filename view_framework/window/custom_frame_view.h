
#ifndef __custom_frame_view_h__
#define __custom_frame_view_h__

#pragma once

#include "../controls/button/image_button.h"
#include "non_client_view.h"
#include "window.h"

namespace gfx
{
    class Font;
    class Size;
    class Path;
    class Point;
}

namespace view
{

    ///////////////////////////////////////////////////////////////////////////////
    //
    // CustomFrameView
    //
    //  A ChromeView that provides the non client frame for Windows. This means
    //  rendering the non-standard window caption, border, and controls.
    //
    ////////////////////////////////////////////////////////////////////////////////
    class CustomFrameView : public NonClientFrameView, public ButtonListener
    {
    public:
        explicit CustomFrameView(Window* frame);
        virtual ~CustomFrameView();

        // Overridden from views::NonClientFrameView:
        virtual gfx::Rect GetBoundsForClientView() const;
        virtual gfx::Rect GetWindowBoundsForClientBounds(
            const gfx::Rect& client_bounds) const;
        virtual int NonClientHitTest(const gfx::Point& point);
        virtual void GetWindowMask(const gfx::Size& size, gfx::Path* window_mask);
        virtual void EnableClose(bool enable);
        virtual void ResetWindowControls();

        // View overrides:
        virtual void Paint(gfx::Canvas* canvas);
        virtual void Layout();
        virtual gfx::Size GetPreferredSize();

        // ButtonListener implementation:
        virtual void ButtonPressed(Button* sender, const Event& event);

    private:
        // Returns the thickness of the border that makes up the window frame edges.
        // This does not include any client edge.
        int FrameBorderThickness() const;

        // Returns the thickness of the entire nonclient left, right, and bottom
        // borders, including both the window frame and any client edge.
        int NonClientBorderThickness() const;

        // Returns the height of the entire nonclient top border, including the window
        // frame, any title area, and any connected client edge.
        int NonClientTopBorderHeight() const;

        // Returns the y-coordinate of the caption buttons.
        int CaptionButtonY() const;

        // Returns the thickness of the nonclient portion of the 3D edge along the
        // bottom of the titlebar.
        int TitlebarBottomThickness() const;

        // Returns the size of the titlebar icon.  This is used even when the icon is
        // not shown, e.g. to set the titlebar height.
        int IconSize() const;

        // Returns the bounds of the titlebar icon (or where the icon would be if
        // there was one).
        gfx::Rect IconBounds() const;

        // Returns true if the client edge should be drawn. This is true if
        // the window delegate wants a client edge and we are not maxmized.
        bool ShouldShowClientEdge() const;

        // Paint various sub-components of this view.
        void PaintRestoredFrameBorder(gfx::Canvas* canvas);
        void PaintMaximizedFrameBorder(gfx::Canvas* canvas);
        void PaintTitleBar(gfx::Canvas* canvas);
        void PaintRestoredClientEdge(gfx::Canvas* canvas);

        // Layout various sub-components of this view.
        void LayoutWindowControls();
        void LayoutTitleBar();
        void LayoutClientView();

        // The bounds of the client view, in this view's coordinates.
        gfx::Rect client_view_bounds_;

        // The layout rect of the title, if visible.
        gfx::Rect title_bounds_;

        // Window controls.
        ImageButton* close_button_;
        ImageButton* restore_button_;
        ImageButton* maximize_button_;
        ImageButton* minimize_button_;
        ImageButton* window_icon_;
        bool should_show_minmax_buttons_;
        bool should_show_client_edge_;

        // The window that owns this view.
        Window* frame_;

        // Initialize various static resources.
        static void InitClass();
        static gfx::Font* title_font_;

        DISALLOW_COPY_AND_ASSIGN(CustomFrameView);
    };

} //namespace view

#endif //__custom_frame_view_h__
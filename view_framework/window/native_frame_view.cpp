
#include "native_frame_view.h"

#include "window_win.h"

namespace view
{

    ////////////////////////////////////////////////////////////////////////////////
    // NativeFrameView, public:

    NativeFrameView::NativeFrameView(WindowWin* frame)
        : NonClientFrameView(),
        frame_(frame) {}

    NativeFrameView::~NativeFrameView() {}

    ////////////////////////////////////////////////////////////////////////////////
    // NativeFrameView, NonClientFrameView overrides:

    gfx::Rect NativeFrameView::GetBoundsForClientView() const
    {
        return gfx::Rect(0, 0, width(), height());
    }

    gfx::Rect NativeFrameView::GetWindowBoundsForClientBounds(
        const gfx::Rect& client_bounds) const
    {
        RECT rect = client_bounds.ToRECT();
        AdjustWindowRectEx(&rect, frame_->window_style(), FALSE,
            frame_->window_ex_style());
        return gfx::Rect(rect);
    }

    int NativeFrameView::NonClientHitTest(const gfx::Point& point)
    {
        return frame_->GetClientView()->NonClientHitTest(point);
    }

    void NativeFrameView::GetWindowMask(const gfx::Size& size,
        gfx::Path* window_mask)
    {
        // Nothing to do, we use the default window mask.
    }

    void NativeFrameView::EnableClose(bool enable)
    {
        // Nothing to do, handled automatically by Window.
    }

    void NativeFrameView::ResetWindowControls()
    {
        // Nothing to do.
    }

    gfx::Size NativeFrameView::GetPreferredSize()
    {
        gfx::Size pref = frame_->GetClientView()->GetPreferredSize();
        gfx::Rect bounds(0, 0, pref.width(), pref.height());
        return frame_->GetNonClientView()->GetWindowBoundsForClientBounds(
            bounds).size();
    }

} //namespace view
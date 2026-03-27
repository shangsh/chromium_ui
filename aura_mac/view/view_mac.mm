#ifdef AURA_MAC

#include "view/view_mac.h"

namespace aura {
namespace mac {

ViewMac::ViewMac() : nsview_(nil), visible_(true), enabled_(true) {
}

ViewMac::~ViewMac() {
}

void ViewMac::SetBounds(int x, int y, int width, int height) {
    if (nsview_) {
        [nsview_ setFrame:NSMakeRect(x, y, width, height)];
    }
}

void ViewMac::SetVisible(bool visible) {
    visible_ = visible;
    if (nsview_) {
        [nsview_ setHidden:!visible];
    }
}

void ViewMac::SetEnabled(bool enabled) {
    enabled_ = enabled;
}

void ViewMac::AddChild(ViewMac* child) {
    if (child && child->nsview_ && nsview_) {
        [nsview_ addSubview:child->nsview_];
        children_.push_back(child);
    }
}

void ViewMac::RemoveChild(ViewMac* child) {
    // Remove from children vector
}

void ViewMac::SetNSView(NSView* view) {
    nsview_ = view;
}

// WidgetMac implementation
WidgetMac::WidgetMac() : window_(nil), nsview_(nil), content_view_(nullptr), visible_(false) {
}

WidgetMac::~WidgetMac() {
    if (window_) {
        [window_ close];
    }
}

bool WidgetMac::Init() {
    bounds_ = gfx::Rect(0, 0, 800, 600);
    window_ = [[NSWindow alloc] initWithContentRect:NSMakeRect(0, 0, 800, 600)
                                         styleMask:NSWindowStyleMaskTitled | NSWindowStyleMaskClosable |
                                                  NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable
                                           backing:NSBackingStoreBuffered
                                             defer:NO];
    if (!window_) return false;
    
    nsview_ = [[NSView alloc] initWithFrame:NSMakeRect(0, 0, 800, 600)];
    [window_ setContentView:nsview_];
    [window_ center];
    
    content_view_ = new ViewMac();
    content_view_->SetNSView(nsview_);
    
    return true;
}

void WidgetMac::Show() {
    visible_ = true;
    if (window_) {
        [window_ orderFront:nil];
    }
}

void WidgetMac::Hide() {
    visible_ = false;
    if (window_) {
        [window_ orderOut:nil];
    }
}

void WidgetMac::Close() {
    if (window_) {
        [window_ close];
    }
}

void WidgetMac::SetBounds(const gfx::Rect& bounds) {
    bounds_ = bounds;
    if (window_) {
        [window_ setFrame:NSMakeRect(bounds.x(), bounds.y(), bounds.width(), bounds.height()) display:YES];
    }
}

gfx::Rect WidgetMac::GetBounds() const {
    return bounds_;
}

void WidgetMac::SetContentView(ViewMac* view) {
    content_view_ = view;
    if (view && nsview_) {
        view->SetNSView(nsview_);
    }
}

ViewMac* WidgetMac::GetContentView() const {
    return content_view_;
}

void WidgetMac::SetTitle(const std::wstring& title) {
    title_ = title;
    if (window_) {
        NSString* nsTitle = [NSString stringWithUTF8String:std::string(title.begin(), title.end()).c_str()];
        [window_ setTitle:nsTitle];
    }
}

} // namespace mac
} // namespace aura

#endif // AURA_MAC

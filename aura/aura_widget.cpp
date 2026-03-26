#include "aura/aura_widget.h"

namespace aura {

Widget::Widget()
    : window_(nil),
      nsview_(nil),
      content_view_(nullptr),
      delegate_(nullptr),
      visible_(false) {
}

Widget::~Widget() {
    if (window_) {
        [window_ close];
    }
    delete content_view_;
}

bool Widget::Init() {
    window_ = [[NSWindow alloc] initWithContentRect:NSMakeRect(0, 0, 800, 600)
                                          styleMask:NSWindowStyleMaskTitled | NSWindowStyleMaskClosable |
                                                   NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable
                                            backing:NSBackingStoreBuffered
                                              defer:NO];
    if (!window_) return false;
    
    nsview_ = [[NSView alloc] initWithFrame:NSMakeRect(0, 0, 800, 600)];
    [window_ setContentView:nsview_];
    [window_ center];
    
    content_view_ = new View();
    content_view_->SetNativeView(nsview_);
    
    return true;
}

bool Widget::InitAsChild(NSView* parent) {
    nsview_ = [[NSView alloc] initWithFrame:NSMakeRect(0, 0, 100, 100)];
    if (!nsview_) return false;
    
    if (parent) {
        [parent addSubview:nsview_];
    }
    
    content_view_ = new View();
    content_view_->SetNativeView(nsview_);
    
    return true;
}

void Widget::Show() {
    visible_ = true;
    if (window_) {
        [window_ orderFront:nil];
    }
}

void Widget::Hide() {
    visible_ = false;
    if (window_) {
        [window_ orderOut:nil];
    }
}

void Widget::Close() {
    if (delegate_) {
        delegate_->OnWidgetClose();
    }
    if (window_) {
        [window_ close];
    }
}

bool Widget::IsVisible() const {
    return visible_;
}

void Widget::SetBounds(const gfx::Rect& bounds) {
    bounds_ = bounds;
    if (window_) {
        [window_ setFrame:NSMakeRect(bounds.x(), bounds.y(), bounds.width(), bounds.height()) display:YES];
    }
    if (nsview_) {
        [nsview_ setFrame:NSMakeRect(0, 0, bounds.width(), bounds.height())];
    }
}

gfx::Rect Widget::GetBounds() const {
    return bounds_;
}

void Widget::SetSize(const gfx::Size& size) {
    SetBounds(gfx::Rect(bounds_.x(), bounds_.y(), size.width(), size.height()));
}

gfx::Size Widget::GetSize() const {
    return bounds_.size();
}

void Widget::SetContentView(View* view) {
    content_view_ = view;
    if (view && nsview_) {
        view->SetNativeView(nsview_);
    }
}

void Widget::SetTitle(const std::wstring& title) {
    title_ = title;
    if (window_) {
        NSString* nsTitle = [NSString stringWithUTF8String:std::string(title.begin(), title.end()).c_str()];
        [window_ setTitle:nsTitle];
    }
}

std::wstring Widget::GetTitle() const {
    return title_;
}

} // namespace aura

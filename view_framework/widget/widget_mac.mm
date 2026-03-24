#include "view_framework/widget/widget_mac.h"

#include "view_framework/window/window.h"
#include "view_framework/widget/widget_delegate.h"

namespace view
{

Widget::Widget() 
    : view_(nil), 
      window_(nullptr), 
      delegate_(nullptr),
      is_visible_(false) 
{
}

Widget::~Widget()
{
    if (view_)
    {
        [view_ release];
        view_ = nil;
    }
}

bool Widget::Init()
{
    view_ = [[NSView alloc] initWithFrame:NSMakeRect(0, 0, 100, 100)];
    return view_ != nil;
}

bool Widget::InitAsChild(NSView* parent)
{
    if (!Init())
        return false;
    
    if (parent)
    {
        [parent addSubview:view_];
    }
    return true;
}

void Widget::Show()
{
    if (view_)
    {
        [view_ setHidden:NO];
        is_visible_ = true;
    }
}

void Widget::Hide()
{
    if (view_)
    {
        [view_ setHidden:YES];
        is_visible_ = false;
    }
}

void Widget::Close()
{
    if (view_)
    {
        [view_ removeFromSuperview];
    }
}

bool Widget::IsVisible() const
{
    return is_visible_;
}

void Widget::SetBounds(const gfx::Rect& bounds)
{
    bounds_ = bounds;
    if (view_)
    {
        [view_ setFrame:NSMakeRect(bounds.x(), bounds.y(), bounds.width(), bounds.height())];
    }
}

gfx::Rect Widget::GetBounds() const
{
    return bounds_;
}

void Widget::SetSize(const gfx::Size& size)
{
    SetBounds(gfx::Rect(bounds_.x(), bounds_.y(), size.width(), size.height()));
}

gfx::Size Widget::GetSize() const
{
    return bounds_.size();
}

void Widget::SetPosition(const gfx::Point& position)
{
    SetBounds(gfx::Rect(position.x(), position.y(), bounds_.width(), bounds_.height()));
}

gfx::Point Widget::GetPosition() const
{
    return bounds_.origin();
}

bool Widget::IsWindow() const
{
    return window_ != nullptr;
}

Window* Widget::GetWindow()
{
    return window_;
}

void Widget::SetWindow(Window* window)
{
    window_ = window;
}

} // namespace view

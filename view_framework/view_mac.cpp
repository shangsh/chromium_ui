#include "view_framework/view_mac.h"

#include <algorithm>

#include "gfx/canvas.h"

namespace view
{

View::View()
    : id_(0),
      visible_(true),
      enabled_(true),
      parent_(nullptr),
      ns_view_(nullptr)
{
}

View::~View()
{
    for (View* child : children_)
    {
        delete child;
    }
    children_.clear();
}

void View::Init()
{
}

void View::ViewHierarchyChanged(bool is_add, View* parent, View* child)
{
}

void View::set_bounds(int x, int y, int width, int height)
{
    bounds_ = gfx::Rect(x, y, width, height);
    if (ns_view_)
    {
        [ns_view_ setFrame:NSMakeRect(x, y, width, height)];
    }
    Invalidate();
}

void View::set_x(int x)
{
    set_bounds(x, bounds_.y(), bounds_.width(), bounds_.height());
}

void View::set_y(int y)
{
    set_bounds(bounds_.x(), y, bounds_.width(), bounds_.height());
}

void View::set_width(int width)
{
    set_bounds(bounds_.x(), bounds_.y(), width, bounds_.height());
}

void View::set_height(int height)
{
    set_bounds(bounds_.x(), bounds_.y(), bounds_.width(), height);
}

void View::Layout()
{
    for (View* child : children_)
    {
        if (child->visible())
        {
            child->Layout();
        }
    }
}

void View::Invalidate()
{
    // Trigger redraw on macOS
    if (ns_view_)
    {
        [ns_view_ setNeedsDisplay:YES];
    }
}

gfx::Size View::GetPreferredSize() const
{
    return size();
}

gfx::Size View::GetMinimumSize() const
{
    return gfx::Size(0, 0);
}

void View::SetVisible(bool visible)
{
    visible_ = visible;
    if (ns_view_)
    {
        [ns_view_ setHidden:!visible];
    }
}

void View::AddChildView(View* child)
{
    if (child && std::find(children_.begin(), children_.end(), child) == children_.end())
    {
        children_.push_back(child);
        child->parent_ = this;
        child->ViewHierarchyChanged(true, this, child);
    }
}

void View::RemoveChildView(View* child)
{
    if (child)
    {
        auto it = std::find(children_.begin(), children_.end(), child);
        if (it != children_.end())
        {
            children_.erase(it);
            child->parent_ = nullptr;
            child->ViewHierarchyChanged(false, this, child);
        }
    }
}

void View::SetNativeView(NSView* view)
{
    ns_view_ = view;
    if (ns_view_)
    {
        [ns_view_ setFrame:NSMakeRect(bounds_.x(), bounds_.y(), 
                                       bounds_.width(), bounds_.height())];
    }
}

void View::SetEnabled(bool enabled)
{
    enabled_ = enabled;
    if (ns_view_)
    {
        [ns_view_ setEnabled:enabled];
    }
}

bool View::IsFocusable() const
{
    return visible_ && enabled_;
}

void View::RequestFocus()
{
    if (ns_view_)
    {
        [ns_view_ becomeFirstResponder];
    }
}

std::wstring View::GetTooltipText() const
{
    return tooltip_text_;
}

void View::Paint(gfx::Canvas* canvas)
{
    // Default: do nothing, subclasses override
}

void View::OnPaint(gfx::Canvas* canvas)
{
    Paint(canvas);
}

} // namespace view

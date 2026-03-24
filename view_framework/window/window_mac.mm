#include "view_framework/window/window_mac.h"

#include <string>

#include "view_framework/window/window_delegate.h"

namespace view
{

Window::Window() 
    : window_(nil), 
      delegate_(nullptr), 
      is_active_(false) 
{
}

Window::~Window()
{
    if (window_)
    {
        [window_ release];
        window_ = nil;
    }
}

bool Window::Init()
{
    NSRect frame = NSMakeRect(0, 0, 800, 600);
    NSWindowStyleMask style = NSTitledWindowMask | NSClosableWindowMask | 
                              NSMiniaturizableWindowMask | NSResizableWindowMask;
    
    window_ = [[NSWindow alloc] initWithContentRect:frame
                                           styleMask:style
                                             backing:NSBackingStoreBuffered
                                               defer:NO];
    
    if (!window_)
        return false;

    [window_ setTitle:@"Chromium UI Window"];
    [window_ center];
    [window_ makeKeyAndOrderFront:nil];
    
    bounds_ = gfx::Rect(0, 0, 800, 600);
    return true;
}

void Window::Show()
{
    if (window_)
    {
        [window_ orderFront:nil];
    }
}

void Window::Hide()
{
    if (window_)
    {
        [window_ orderOut:nil];
    }
}

void Window::Close()
{
    if (window_)
    {
        [window_ close];
    }
}

void Window::SetBounds(const gfx::Rect& bounds)
{
    bounds_ = bounds;
    if (window_)
    {
        NSRect frame = NSMakeRect(bounds.x(), bounds.y(), bounds.width(), bounds.height());
        [window_ setFrame:frame display:YES];
    }
}

gfx::Rect Window::GetBounds() const
{
    return bounds_;
}

void Window::SetSize(const gfx::Size& size)
{
    SetBounds(gfx::Rect(bounds_.x(), bounds_.y(), size.width(), size.height()));
}

gfx::Size Window::GetSize() const
{
    return bounds_.size();
}

void Window::SetPosition(const gfx::Point& position)
{
    SetBounds(gfx::Rect(position.x(), position.y(), bounds_.width(), bounds_.height()));
}

gfx::Point Window::GetPosition() const
{
    return bounds_.origin();
}

void Window::SetTitle(const std::wstring& title)
{
    title_ = title;
    if (window_)
    {
        NSString* nsTitle = [NSString stringWithUTF8String:std::string(title.begin(), title.end()).c_str()];
        [window_ setTitle:nsTitle];
    }
}

std::wstring Window::GetTitle() const
{
    return title_;
}

void Window::Activate()
{
    if (window_)
    {
        [window_ makeKeyAndOrderFront:nil];
        is_active_ = true;
    }
}

void Window::Deactivate()
{
    is_active_ = false;
}

bool Window::IsActive() const
{
    return is_active_;
}

} // namespace view

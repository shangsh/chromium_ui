#ifndef VIEW_FRAMEWORK_WINDOW_H_
#define VIEW_FRAMEWORK_WINDOW_H_

#import <Cocoa/Cocoa.h>

#include <string>
#include "gfx/rect.h"
#include "gfx/size.h"

namespace view
{

class WindowDelegate;

class Window
{
public:
    Window();
    ~Window();

    // Create the window
    bool Init();

    // Show/hide
    void Show();
    void Hide();
    void Close();

    // Size and position
    void SetBounds(const gfx::Rect& bounds);
    gfx::Rect GetBounds() const;
    void SetSize(const gfx::Size& size);
    gfx::Size GetSize() const;
    void SetPosition(const gfx::Point& position);
    gfx::Point GetPosition() const;

    // Title
    void SetTitle(const std::wstring& title);
    std::wstring GetTitle() const;

    // Activate
    void Activate();
    void Deactivate();
    bool IsActive() const;

    // Window delegate
    void set_delegate(WindowDelegate* delegate) { delegate_ = delegate; }
    WindowDelegate* delegate() const { return delegate_; }

    // Native window handle
    NSWindow* native_window() const { return window_; }

private:
    NSWindow* window_;
    WindowDelegate* delegate_;
    gfx::Rect bounds_;
    std::wstring title_;
    bool is_active_;
};

} // namespace view

#endif // VIEW_FRAMEWORK_WINDOW_H_

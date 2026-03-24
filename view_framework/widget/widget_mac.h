#ifndef VIEW_FRAMEWORK_WIDGET_WIDGET_H_
#define VIEW_FRAMEWORK_WIDGET_WIDGET_H_

#import <Cocoa/Cocoa.h>

#include <string>
#include "gfx/rect.h"
#include "gfx/size.h"

namespace view
{

class WidgetDelegate;

class Widget
{
public:
    Widget();
    ~Widget();

    // Init
    bool Init();
    bool InitAsChild(NSView* parent);

    // Show/hide
    void Show();
    void Hide();
    void Close();
    bool IsVisible() const;

    // Size and position
    void SetBounds(const gfx::Rect& bounds);
    gfx::Rect GetBounds() const;
    void SetSize(const gfx::Size& size);
    gfx::Size GetSize() const;
    void SetPosition(const gfx::Point& position);
    gfx::Point GetPosition() const;

    // Window
    bool IsWindow() const;
    class Window* GetWindow();
    void SetWindow(Window* window);

    // Native view
    NSView* native_view() const { return view_; }

    // Widget delegate
    void set_delegate(WidgetDelegate* delegate) { delegate_ = delegate; }
    WidgetDelegate* delegate() const { return delegate_; }

private:
    NSView* view_;
    Window* window_;
    WidgetDelegate* delegate_;
    gfx::Rect bounds_;
    bool is_visible_;
};

} // namespace view

#endif // VIEW_FRAMEWORK_WIDGET_WIDGET_H_

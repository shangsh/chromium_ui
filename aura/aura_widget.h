#ifndef AURA_WIDGET_H_
#define AURA_WIDGET_H_

#import <Cocoa/Cocoa.h>

#include "aura/aura_view.h"
#include <string>

namespace aura {

class WidgetDelegate {
public:
    virtual void OnWidgetClose() {}
    virtual void OnWidgetActivate() {}
};

class Widget {
public:
    Widget();
    ~Widget();
    
    // Init
    bool Init();
    bool InitAsChild(NSView* parent);
    
    // Show/Hide
    void Show();
    void Hide();
    void Close();
    bool IsVisible() const;
    
    // Size/Position
    void SetBounds(const gfx::Rect& bounds);
    gfx::Rect GetBounds() const;
    void SetSize(const gfx::Size& size);
    gfx::Size GetSize() const;
    
    // Content
    void SetContentView(View* view);
    View* GetContentView() const { return content_view_; }
    
    // Native
    NSWindow* native_window() const { return window_; }
    NSView* native_view() const { return content_view_ ? content_view_->native_view() : nullptr; }
    
    // Delegate
    void set_delegate(WidgetDelegate* d) { delegate_ = d; }
    WidgetDelegate* delegate() const { return delegate_; }
    
    // Title
    void SetTitle(const std::wstring& title);
    std::wstring GetTitle() const;

private:
    NSWindow* window_;
    NSView* nsview_;
    View* content_view_;
    WidgetDelegate* delegate_;
    gfx::Rect bounds_;
    bool visible_;
    std::wstring title_;
};

} // namespace aura

#endif

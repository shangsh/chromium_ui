#ifndef AURA_MAC_VIEW_H_
#define AURA_MAC_VIEW_H_

// macOS-specific Aura View implementation
// This file only compiled on macOS with -DAURA_MAC flag

#ifdef AURA_MAC

#import <Cocoa/Cocoa.h>

#include <string>
#include <vector>

// Forward declare aura types
namespace aura {
    class View;
}

#include "../../gfx/rect.h"
#include "../../gfx/size.h"
#include "../../gfx/insets.h"

namespace aura {
namespace mac {

// View - macOS implementation
class ViewMac {
public:
    ViewMac();
    virtual ~ViewMac();
    
    virtual void SetBounds(int x, int y, int width, int height);
    void SetVisible(bool visible);
    void SetEnabled(bool enabled);
    void AddChild(ViewMac* child);
    void RemoveChild(ViewMac* child);
    
    NSView* GetNSView() const { return nsview_; }
    void SetNSView(NSView* view);
    
private:
    NSView* nsview_;
    bool visible_;
    bool enabled_;
    std::vector<ViewMac*> children_;
};

// Widget - macOS implementation  
class WidgetMac {
public:
    WidgetMac();
    ~WidgetMac();
    
    bool Init();
    void Show();
    void Hide();
    void Close();
    
    void SetBounds(const gfx::Rect& bounds);
    gfx::Rect GetBounds() const;
    
    void SetContentView(ViewMac* view);
    ViewMac* GetContentView() const;
    
    NSWindow* GetNSWindow() const { return window_; }
    
    void SetTitle(const std::wstring& title);
    
private:
    NSWindow* window_;
    NSView* nsview_;
    ViewMac* content_view_;
    gfx::Rect bounds_;
    bool visible_;
    std::wstring title_;
};

} // namespace mac
} // namespace aura

#endif // AURA_MAC
#endif // AURA_MAC_VIEW_H_

#ifndef VIEW_FRAMEWORK_VIEW_MAC_H_
#define VIEW_FRAMEWORK_VIEW_MAC_H_

#import <Cocoa/Cocoa.h>

#include <string>
#include <vector>
#include "gfx/rect.h"
#include "gfx/size.h"
#include "gfx/insets.h"

namespace view
{

class View
{
public:
    View();
    virtual ~View();

    // Initialize
    virtual void Init();
    virtual void ViewHierarchyChanged(bool is_add, View* parent, View* child);

    // Properties
    void set_bounds(int x, int y, int width, int height);
    void set_x(int x);
    void set_y(int y);
    void set_width(int width);
    void set_height(int height);
    gfx::Rect bounds() const { return bounds_; }
    gfx::Size size() const { return bounds_.size(); }
    gfx::Point origin() const { return bounds_.origin(); }

    // Layout
    virtual void Layout();
    virtual void Invalidate();
    virtual gfx::Size GetPreferredSize() const;
    virtual gfx::Size GetMinimumSize() const;

    // Visibility
    virtual void SetVisible(bool visible);
    bool visible() const { return visible_; }

    // Parent/child
    View* parent() const { return parent_; }
    void AddChildView(View* child);
    void RemoveChildView(View* child);
    const std::vector<View*>& children() const { return children_; }
    int child_count() const { return static_cast<int>(children_.size()); }
    View* child_at(int index) const { return children_[index]; }

    // Native view
    NSView* native_view() const { return ns_view_; }
    void SetNativeView(NSView* view);

    // ID
    void set_id(int id) { id_ = id; }
    int id() const { return id_; }

    // Enabled
    void SetEnabled(bool enabled);
    bool enabled() const { return enabled_; }

    // Focus
    virtual bool IsFocusable() const;
    virtual void RequestFocus();

    // Tooltip
    virtual std::wstring GetTooltipText() const;
    void set_tooltip_text(const std::wstring& text) { tooltip_text_ = text; }

protected:
    // Paint
    virtual void Paint(gfx::Canvas* canvas);
    virtual void OnPaint(gfx::Canvas* canvas);

private:
    int id_;
    gfx::Rect bounds_;
    gfx::Insets insets_;
    bool visible_;
    bool enabled_;
    View* parent_;
    std::vector<View*> children_;
    NSView* ns_view_;
    std::wstring tooltip_text_;
};

} // namespace view

#endif // VIEW_FRAMEWORK_VIEW_MAC_H_

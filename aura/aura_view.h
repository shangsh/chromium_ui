#ifndef AURA_VIEW_H_
#define AURA_VIEW_H_

#import <Cocoa/Cocoa.h>

#include <string>
#include <vector>
#include "gfx/rect.h"
#include "gfx/size.h"
#include "gfx/insets.h"

namespace aura {

// Forward declarations
class View;
class Widget;

// Background
class Background {
public:
    static Background* CreateSolidBackground(int r, int g, int b);
    static Background* CreateSystemColor(int color_id);
    NSColor* nsColor() const { return color_; }
private:
    NSColor* color_;
};

// Border  
class Border {
public:
    static Border* CreateSolidBorder(int width);
    int width() const { return width_; }
private:
    int width_;
};

// View class - core of Aura UI
class View {
public:
    View();
    virtual ~View();
    
    // Hierarchy
    virtual void AddChildView(View* child);
    virtual void RemoveChildView(View* child);
    View* parent() const { return parent_; }
    const std::vector<View*>& children() const { return children_; }
    int child_count() const { return (int)children_.size(); }
    View* child_at(int index) const { return children_[index]; }
    
    // Bounds
    virtual void SetBounds(int x, int y, int width, int height);
    virtual void SetPosition(int x, int y);
    virtual void SetSize(int width, int height);
    gfx::Rect bounds() const { return bounds_; }
    gfx::Size size() const { return bounds_.size(); }
    gfx::Point origin() const { return bounds_.origin(); }
    
    // Visibility
    void SetVisible(bool visible);
    bool IsVisible() const { return visible_; }
    
    // Enabled
    void SetEnabled(bool enabled);
    bool IsEnabled() const { return enabled_; }
    
    // Focus
    void SetFocusable(bool focusable);
    bool IsFocusable() const { return focusable_; }
    virtual void RequestFocus();
    
    // ID
    void set_id(int id) { id_ = id; }
    int id() const { return id_; }
    
    // Native
    NSView* native_view() const { return nsview_; }
    void SetNativeView(NSView* view);
    
    // Paint (override in subclasses)
    virtual void OnPaint();
    
    // Layout
    virtual void Invalidate();
    virtual void Layout();
    virtual gfx::Size GetPreferredSize() const;
    
protected:
    int id_;
    gfx::Rect bounds_;
    bool visible_;
    bool enabled_;
    bool focusable_;
    View* parent_;
    std::vector<View*> children_;
    NSView* nsview_;
    Background* background_;
    Border* border_;
};

// Button listener
class ButtonListener {
public:
    virtual void OnButtonClick() = 0;
};

// Button - inherits from View
class Button : public View {
public:
    Button();
    virtual ~Button();
    
    void SetText(const std::wstring& text);
    std::wstring GetText() const { return text_; }
    
    void SetIcon(NSImage* icon);
    
    void SetListener(ButtonListener* listener) { listener_ = listener; }
    ButtonListener* GetListener() const { return listener_; }
    
    NSButton* native_button() const { return button_; }
    
    virtual gfx::Size GetPreferredSize() const override;
    virtual void SetBounds(int x, int y, int width, int height) override;
    
private:
    NSButton* button_;
    std::wstring text_;
    ButtonListener* listener_;
};

// Label - inherits from View  
class Label : public View {
public:
    Label();
    Label(const std::wstring& text);
    virtual ~Label();
    
    void SetText(const std::wstring& text);
    std::wstring GetText() const { return text_; }
    
    void SetFont(NSFont* font);
    void SetTextColor(NSColor* color);
    void SetAlignment(NSTextAlignment align);
    
    virtual gfx::Size GetPreferredSize() const override;
    virtual void SetBounds(int x, int y, int width, int height) override;
    
private:
    NSTextField* label_;
    std::wstring text_;
};

// TextField - inherits from View
class TextField : public View {
public:
    TextField();
    virtual ~TextField();
    
    void SetText(const std::wstring& text);
    std::wstring GetText() const;
    
    void SetPlaceholder(const std::wstring& text);
    
    void SetPasswordMode(bool password);
    bool IsPasswordMode() const { return password_; }
    
    void SetReadOnly(bool read_only);
    bool IsReadOnly() const { return read_only_; }
    
    NSTextField* native_textfield() const { return textfield_; }
    
    virtual gfx::Size GetPreferredSize() const override;
    virtual void SetBounds(int x, int y, int width, int height) override;
    
private:
    NSTextField* textfield_;
    std::wstring placeholder_;
    bool password_;
    bool read_only_;
};

// Checkbox - inherits from View
class Checkbox : public View {
public:
    Checkbox();
    Checkbox(const std::wstring& text);
    virtual ~Checkbox();
    
    void SetText(const std::wstring& text);
    std::wstring GetText() const { return text_; }
    
    void SetChecked(bool checked);
    bool IsChecked() const;
    
    NSButton* native_checkbox() const { return checkbox_; }
    
    virtual gfx::Size GetPreferredSize() const override;
    virtual void SetBounds(int x, int y, int width, int height) override;
    
private:
    NSButton* checkbox_;
    std::wstring text_;
};

} // namespace aura

#endif

#ifndef __window_impl_h__
#define __window_impl_h__

#include "base/win_types.h"
#include "rect.h"
#include "size.h"
#include <string>

namespace gfx {

class WindowImpl {
public:
    WindowImpl();
    virtual ~WindowImpl();
    
    bool Create();
    void Show(int cmd_show = 0);
    void Close();
    bool Destroy();
    void* GetHandle() const;
    void SetFocus();
    bool IsVisible() const;
    void SetVisible(bool visible);
    
protected:
    virtual LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
    
private:
    HWND hwnd_;
    bool is_visible_;
};

} // namespace gfx

#endif

#include "window_impl.h"

namespace gfx {

WindowImpl::WindowImpl() {}
WindowImpl::~WindowImpl() {}

bool WindowImpl::Create() { return true; }
void WindowImpl::Show(int cmd_show) {}
void WindowImpl::Close() {}
bool WindowImpl::Destroy() { return true; }
void* WindowImpl::GetHandle() const { return nullptr; }
void WindowImpl::SetFocus() {}
bool WindowImpl::IsVisible() const { return true; }
void WindowImpl::SetVisible(bool visible) {}

} // namespace gfx

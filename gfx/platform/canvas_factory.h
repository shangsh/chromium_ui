#ifndef GFX_PLATFORM_CANVAS_H_
#define GFX_PLATFORM_CANVAS_H_

#pragma once

#include "gfx/canvas.h"

#if PLATFORM_WINDOWS
#include "gfx/platform/canvas_win.h"
#elif PLATFORM_MACOS
#include "gfx/platform/canvas_mac.h"
#elif defined(PLATFORM_LINUX)
#include "gfx/platform/canvas_linux.h"
#endif

namespace gfx
{

// Factory function to create platform-specific canvas
Canvas *Canvas::CreateCanvas() {
#if PLATFORM_WINDOWS
    return new CanvasWin();
#elif PLATFORM_MACOS
    return new CanvasMac();
#else
    return nullptr;
#endif
}

Canvas *Canvas::CreateCanvas(int width, int height, bool is_opaque) {
#if PLATFORM_WINDOWS
    return new CanvasWin(width, height, is_opaque);
#elif PLATFORM_MACOS
    return new CanvasMac(width, height, is_opaque);
#else
    return nullptr;
#endif
}

CanvasPaint *CanvasPaint::CreateCanvasPaint(void* view) {
#if PLATFORM_WINDOWS
    return new CanvasPaintWin((HWND)view);
#elif PLATFORM_MACOS
    return new CanvasPaintMac(view);
#else
    return nullptr;
#endif
}

} // namespace gfx

#endif // GFX_PLATFORM_CANVAS_H_

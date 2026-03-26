#include "gdiplus_initializer.h"

#ifdef _WIN32
#include <windows.h>
#include <gdiplus.h>

namespace Gdiplus {
    inline unsigned int GdiplusStartup(GdiplusStartupInput* input, void* token = nullptr, void* hook = nullptr);
    inline void GdiplusShutdown(void* token);
}

namespace gfx {

GdiplusInitializer::GdiplusInitializer() {
}

GdiplusInitializer::~GdiplusInitializer() {
}

bool GdiplusInitializer::Init() {
    GdiplusStartupInput input;
    input.GdiplusVersion = 1;
    input.DebugEventCallback = nullptr;
    input.SuppressExternalCodecs = 0;
    input.SuppressShadowing = 0;
    input.No2DPerformance = 0;
    input.NoFontLoading = 0;
    input.NoOfflineRendering = 0;
    input.NoCapture = 0;
    return Gdiplus::Ok == GdiplusStartup(&input, &gdiplus_token_, nullptr);
}

void GdiplusInitializer::UnInit() {
    Gdiplus::GdiplusShutdown(gdiplus_token_);
}

}

#endif // _WIN32

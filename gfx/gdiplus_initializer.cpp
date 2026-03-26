#include "gdiplus_initializer.h"

#ifdef _WIN32
#include <windows.h>
#include <gdiplus.h>
#else

namespace Gdiplus {
    typedef int Status;
    const Status Ok = 0;
    const Status OutOfMemory = 1;
    const Status InvalidParameter = 2;
    const Status NotImplemented = 3;
    const Status Aborted = 4;
    const Status FileNotFound = 5;
    const Status ValueOverflow = 6;
    const Status AccessDenied = 7;
    const Status UnknownImageFormat = 8;
    const Status FontFamilyNotFound = 9;
    const Status FontStyleNotFound = 10;
    const Status NotTrueTypeFont = 11;
    const Status UnsupportedGdiplusVersion = 12;
    const Status GdiplusNotInitialized = 13;
    const Status PropertyNotFound = 14;
    const Status PropertyNotSupported = 15;
    const Status GenericError = 100;
    const Status ObjectBusy = 101;
    const Status InsufficientBuffer = 202;
    
    class GdiplusStartupInput {
    public:
        unsigned int GdiplusVersion;
        void* DebugEventCallback;
        int SuppressExternalCodecs;
        int SuppressShadowing;
        int No2DPerformance;
        int NoFontLoading;
        int NoOfflineRendering;
        int NoCapture;
    };
    
    inline unsigned int GdiplusStartup(GdiplusStartupInput* input, void* token = nullptr, void* hook = nullptr) {
        return Ok;
    }
    
    inline void GdiplusShutdown(void* token) {}
}

namespace gfx {

GdiplusInitializer::GdiplusInitializer() {
}

GdiplusInitializer::~GdiplusInitializer() {
}

bool GdiplusInitializer::Init() {
    return true;
}

void GdiplusInitializer::UnInit() {
}

}
#endif


#include "gdiplus_initializer.h"

#include "base/logging.h"

#pragma comment(lib, "gdiplus.lib")

namespace gfx
{

    GdiplusInitializer::GdiplusInitializer() : gdiplus_token_(0) {}

    GdiplusInitializer::~GdiplusInitializer()
    {
        DCHECK(CalledOnValidThread());

        DCHECK(gdiplus_token_ == 0) << "You should call UnInit manually";
    }

    bool GdiplusInitializer::Init()
    {
        DCHECK(CalledOnValidThread());

        if(gdiplus_token_)
        {
            return true;
        }

        Gdiplus::GdiplusStartupInput gdiplus_startup_input;
        return Gdiplus::GdiplusStartup(&gdiplus_token_,
            &gdiplus_startup_input, NULL) == Gdiplus::Ok;
    }

    void GdiplusInitializer::UnInit()
    {
        DCHECK(CalledOnValidThread());

        if(gdiplus_token_)
        {
            Gdiplus::GdiplusShutdown(gdiplus_token_);
            gdiplus_token_ = 0;
        }
    }

} //namespace gfx

#ifndef __default_theme_provider_h__
#define __default_theme_provider_h__

#pragma once

#include <vector>

#include "base/basic_types.h"

#include "gfx/color.h"

#include "theme_provider.h"

namespace view
{

    class DefaultThemeProvider : public ThemeProvider
    {
    public:
        DefaultThemeProvider() {}
        virtual ~DefaultThemeProvider() {}

        // Overridden from ThemeProvider.
        virtual void Init(Profile* profile) {}
        virtual gfx::Bitmap GetBitmapNamed(int id) const;
        virtual gfx::Color GetColor(int id) const
        {
            // Return debugging-blue.
            return 0xff0000ff;
        }
        virtual bool GetDisplayProperty(int id, int* result) const { return false; }
        virtual bool ShouldUseNativeFrame() const;
        virtual bool HasCustomImage(int id) const { return false; }
        virtual RefCountedMemory* GetRawData(int id) const { return NULL; }

    private:
        DISALLOW_COPY_AND_ASSIGN(DefaultThemeProvider);
    };

} //namespace view

#endif //__default_theme_provider_h__
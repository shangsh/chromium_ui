#ifndef GFX_PLATFORM_FONT_H_
#define GFX_PLATFORM_FONT_H_

#pragma once

#include "gfx/font.h"
#include <string>

namespace gfx
{

class PlatformFont : public base::RefCounted<PlatformFont>
{
public:
    static PlatformFont* CreateDefault();
    static PlatformFont* CreateFromNameAndSize(const std::wstring& name, int size);

#if defined(PLATFORM_WINDOWS)
    static PlatformFont* CreateFromNativeFont(HFONT font);
#endif

    virtual std::wstring GetFontName() const = 0;
    virtual int GetFontSize() const = 0;
    virtual int GetHeight() const = 0;
    virtual int GetBaseline() const = 0;
    virtual int GetAverageCharacterWidth() const = 0;
    virtual int GetStringWidth(const std::wstring& text) const = 0;
    virtual int GetExpectedTextWidth(int length) const = 0;
    virtual int GetStyle() const = 0;
    virtual Font DeriveFont(int size_delta, int style) const = 0;

#if defined(PLATFORM_WINDOWS)
    virtual HFONT GetNativeFont() const = 0;
#endif

#if defined(PLATFORM_MACOS)
    virtual void* GetCTFont() const = 0;
    virtual void* GetNSFont() const = 0;
#endif

protected:
    virtual ~PlatformFont() {}
};

} // namespace gfx

#endif // GFX_PLATFORM_FONT_H_

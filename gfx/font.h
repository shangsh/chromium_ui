
#ifndef __font_h__
#define __font_h__

#pragma once

#include <string>

#include "base/ref_counted.h"

namespace gfx
{

    class PlatformFont;

    // Font�ṩ�ײ�����ķ�װ. �������͸�ֵ����, �ҷǳ�����(���ü���).
    class Font
    {
    public:
        // ������.
        enum FontStyle
        {
            NORMAL = 0,
            BOLD = 1,
            ITALIC = 2,
            UNDERLINED = 4,
        };

        // ����ȱʡ���ֺͷ�������.
        Font();

        // ��¡�������.
        Font(const Font& other);
        Font& operator=(const Font& other);

        // �ӱ������崴��Font����.
        explicit Font(HFONT native_font);

        // ��PlatformFont���󹹽�Font. Font����ӹ�PlatformFont���������Ȩ.
        explicit Font(PlatformFont* platform_font);

        // ��ָ�����ֺ��ֺŴ�������.
        Font(const std::wstring& font_name, int font_size);

        ~Font();

        // �Ӵ��ڵ����巵��һ���µ�����, �ֺ�����size_delta. ����, 5���ص�����
        // ����������5����λ.
        Font DeriveFont(int size_delta) const;

        // �Ӵ��ڵ����巵��һ���µ�����, �ֺ�����size_delta. ����, 5���ص�����
        // ����������5����λ.
        // style����ָ���µ�������, ���⼸��ֵ�����: BOLD��ITALIC��UNDERLINED.
        Font DeriveFont(int size_delta, int style) const;

        // ������ʾ�ַ�����Ĵ�ֱ����. �����о�, ����˵�߶ȿ��ܱ�ascent+descent��.
        int GetHeight() const;

        // ���������baseline����ascent.
        int GetBaseline() const;

        // ���������ƽ���ַ����.
        int GetAverageCharacterWidth() const;

        // ������ʾ�ı������ˮƽ����.
        int GetStringWidth(const std::wstring& text) const;

        // ������ʾָ�������ַ������ˮƽ����. ����GetStringWidth()��ȡʵ����Ŀ.
        int GetExpectedTextWidth(int length) const;

        // ����������.
        int GetStyle() const;

        // ������������.
        const std::wstring& GetFontName() const;

        // �����ֺ�(����).
        int GetFontSize() const;

        // ���ر���������.
        HFONT GetNativeFont() const;

        // ��ȡ�ײ�ƽ̨ʵ��. ���Ը�����Ҫǿת������ʵ������.
        PlatformFont* platform_font() const { return platform_font_.get(); }

    private:
        // ƽ̨�����װʵ��.
        scoped_refptr<PlatformFont> platform_font_;
    };

} //namespace gfx

#endif //__font_h__
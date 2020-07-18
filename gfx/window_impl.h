
#ifndef __window_impl_h__
#define __window_impl_h__

#pragma once

#include <windows.h>

#include <string>

#include "base/logging.h"

#include "rect.h"

namespace gfx
{

    // ʹ����Ϣӳ�������Ҫʵ�ֱ��ӿ�. ProcessWindowMessage�ں�
    // VIEW_BEGIN_MESSAGE_MAP_EX��ʵ��.
    class MessageMapInterface
    {
    public:
        // ��window����Ϣ�����д���һ����Ϣ.
        virtual BOOL ProcessWindowMessage(HWND window,
            UINT message,
            WPARAM w_param,
            LPARAM l_param,
            LRESULT& result,
            DWORD msg_mad_id = 0) = 0;
    };

    ///////////////////////////////////////////////////////////////////////////////
    //
    // WindowImpl
    //  ���װ��HWND����������ϸ��, �Ҷ���������Windowsʹ�õĺ�������.
    //
    ///////////////////////////////////////////////////////////////////////////////
    class WindowImpl : public MessageMapInterface
    {
    public:
        WindowImpl();
        virtual ~WindowImpl();

        // ��ʼ�������ںͳ�ʼ��С.
        void Init(HWND parent, const Rect& bounds);

        // ����ȱʡ����ͼ��, ��û��ָ��ʱʹ��.
        virtual HICON GetDefaultWindowIcon() const;

        // ����Window������HWND.
        HWND hwnd() const { return hwnd_; }

        // ���ô��ڷ��. ֻ���ڴ��ڴ�����ʱ��ʹ��. Ҳ����˵��Init����֮��������,
        // û���κ�Ч��.
        void set_window_style(DWORD style) { window_style_ = style; }
        DWORD window_style() const { return window_style_; }

        // ���ô��ڵ���չ���. �μ�|set_window_style|��ע��.
        void set_window_ex_style(DWORD style) { window_ex_style_ = style; }
        DWORD window_ex_style() const { return window_ex_style_; }

        // ������ʹ�õķ��. ȱʡ��CS_DBLCLKS.
        void set_initial_class_style(UINT class_style)
        {
            // �����Ƕ�̬������, ���Բ�Ҫע��Ϊȫ�ֵ�!
            DCHECK_EQ((class_style&CS_GLOBALCLASS), 0U);
            class_style_ = class_style;
        }
        UINT initial_class_style() const { return class_style_; }

        // ���|hwnd|��һ��WindowImpl, ����true.
        static bool IsWindowImpl(HWND hwnd);

    protected:
        // ���������WndProc�ص�.
        virtual LRESULT OnWndProc(UINT message, WPARAM w_param, LPARAM l_param);

    private:
        friend class ClassRegistrar;

        // ����Windowsʹ�õĴ��ڹ���.
        static LRESULT CALLBACK WndProc(HWND window, UINT message,
            WPARAM w_param, LPARAM l_param);

        // �ڴ���HWNDʱ������ô��ڵ�����. �����������Ҫע�ᴰ����.
        std::wstring GetWindowClassName();

        // ����WidgetWinע�������ǰ׺.
        static const wchar_t* const kBaseClassName;

        // ���ڴ���ʱʹ�õĴ��ڷ��.
        DWORD window_style_;

        // ���ڴ���ʱʹ�õĴ�����չ���.
        DWORD window_ex_style_;

        // ��������.
        UINT class_style_;

        // ���ھ��.
        HWND hwnd_;

        DISALLOW_COPY_AND_ASSIGN(WindowImpl);
    };

} //namespace gfx

#endif //__window_impl_h__
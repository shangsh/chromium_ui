
#ifndef __win_util_h__
#define __win_util_h__

#pragma once

#include <windows.h>

#include <string>

namespace base
{

    void GetNonClientMetrics(NONCLIENTMETRICS* metrics);

    // ���ص�ǰ�û���sid�ַ���.
    bool GetUserSidString(std::wstring* user_sid);

    // ���໯����, ���ر��滻�Ĵ��ڹ���.
    WNDPROC SetWindowProc(HWND hwnd, WNDPROC wndproc);

    // Get/SetWindowLong(..., GWLP_USERDATA, ...)�ķ�װ.
    // ��������ǰ���û�����.
    void* SetWindowUserData(HWND hwnd, void* user_data);
    void* GetWindowUserData(HWND hwnd);

    // ����shift����ǰ�Ƿ񱻰���.
    bool IsShiftPressed();

    // ����ctrl����ǰ�Ƿ񱻰���.
    bool IsCtrlPressed();

    // ����alt����ǰ�Ƿ񱻰���.
    bool IsAltPressed();

    // GetClassNameW API�ķ�װ, ���ش�������. ��ֵ��ʾ��ȡ����ʧ��.
    std::wstring GetClassName(HWND window);

    // ʹ��FormatMessage() API�����ַ���, ʹ��Windowsȱʡ����Ϣ������Դ;
    // ����%ռλ��.
    std::wstring FormatMessage(unsigned messageid);

    // ʹ��GetLastError()���ɿɶ�����Ϣ�ַ���.
    std::wstring FormatLastWin32Error();

} //namespace base

#endif //__win_util_h__
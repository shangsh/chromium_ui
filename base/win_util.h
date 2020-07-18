
#ifndef __win_util_h__
#define __win_util_h__

#pragma once

#include <windows.h>

#include <string>

namespace base
{

    void GetNonClientMetrics(NONCLIENTMETRICS* metrics);

    // 返回当前用户的sid字符串.
    bool GetUserSidString(std::wstring* user_sid);

    // 子类化窗口, 返回被替换的窗口过程.
    WNDPROC SetWindowProc(HWND hwnd, WNDPROC wndproc);

    // Get/SetWindowLong(..., GWLP_USERDATA, ...)的封装.
    // 返回设置前的用户数据.
    void* SetWindowUserData(HWND hwnd, void* user_data);
    void* GetWindowUserData(HWND hwnd);

    // 返回shift键当前是否被按下.
    bool IsShiftPressed();

    // 返回ctrl键当前是否被按下.
    bool IsCtrlPressed();

    // 返回alt键当前是否被按下.
    bool IsAltPressed();

    // GetClassNameW API的封装, 返回窗口类名. 空值表示获取类名失败.
    std::wstring GetClassName(HWND window);

    // 使用FormatMessage() API生成字符串, 使用Windows缺省的消息编译资源;
    // 忽略%占位符.
    std::wstring FormatMessage(unsigned messageid);

    // 使用GetLastError()生成可读的消息字符串.
    std::wstring FormatLastWin32Error();

} //namespace base

#endif //__win_util_h__
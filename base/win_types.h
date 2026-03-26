#ifndef BASE_WIN_TYPES_H_
#define BASE_WIN_TYPES_H_

#ifdef _WIN32
#include <windows.h>
#else

// Windows types and constants - stub definitions for non-Windows platforms
// These are not actually used on macOS - we use native AppKit instead

#include <cstdint>

// Stub types - we use native macOS types instead
typedef void* HWND;
typedef void* HANDLE;
typedef void* HDC;
typedef void* HICON;
typedef void* HCURSOR;
typedef void* HRGN;
typedef void* HINSTANCE;
typedef void* HMODULE;
typedef void* HBRUSH;
typedef void* HFONT;
typedef void* HPEN;
typedef void* HBITMAP;
typedef unsigned int UINT;
typedef intptr_t WPARAM;
typedef intptr_t LPARAM;
typedef intptr_t LRESULT;

// Message (stub)
typedef struct tagMSG {
    void* hwnd;
    unsigned int message;
    WPARAM wParam;
    LPARAM lParam;
    unsigned int time;
    int pt_x;
    int pt_y;
} MSG;

// Constants (stub values)
#define WM_DESTROY 0x0002
#define WM_QUIT 0x0012
#define WM_PAINT 0x000F
#define WM_CREATE 0x0001
#define WM_SIZE 0x0005
#define WM_SETFOCUS 0x0007
#define WM_KILLFOCUS 0x0008
#define WM_CLOSE 0x0010
#define WM_COMMAND 0x0111
#define WM_NOTIFY 0x004E
#define CW_USEDEFAULT 0x80000000

#endif // _WIN32

#endif // BASE_WIN_TYPES_H_

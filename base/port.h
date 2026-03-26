
#ifndef __port_h__
#define __port_h__

#pragma once

#include "build_config.h"

#ifdef _WIN32
#define GG_LONGLONG(x)      x##I64
#define GG_ULONGLONG(x)     x##UI64
#else
#define GG_LONGLONG(x)      x##LL
#define GG_ULONGLONG(x)     x##ULL
#endif

#define GG_INT8_C(x)        (x)
#define GG_INT16_C(x)       (x)
#define GG_INT32_C(x)       (x)
#define GG_INT64_C(x)       GG_LONGLONG(x)

#define GG_UINT8_C(x)       (x ## U)
#define GG_UINT16_C(x)      (x ## U)
#define GG_UINT32_C(x)      (x ## U)
#define GG_UINT64_C(x)      GG_ULONGLONG(x)

#define GG_VA_COPY(a, b)    (a = b)

#ifdef _WIN32
#define API_CALL            __stdcall
#else
#define API_CALL
#endif

#endif //__port_h__
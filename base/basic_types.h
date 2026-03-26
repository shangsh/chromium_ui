#ifndef __basic_types_h__
#define __basic_types_h__

#pragma once

// Fundamental type aliases - defined first so they're available to any
// header that includes basic_types.h (e.g. base/time.h)
#if defined(__LP64__)
typedef long                int64;
typedef unsigned long      uint64;
#else
typedef long long          int64;
typedef unsigned long long  uint64;
#endif
typedef signed char         int8;
typedef unsigned char       uint8;
typedef short               int16;
typedef unsigned short      uint16;
typedef int                 int32;
typedef unsigned int        uint32;
typedef signed int          char32;

// Then pull in platform headers and continue with rest of definitions
// Include time.h early (before port.h) to ensure nanosleep is declared
// before any standard library headers that might need it (e.g. <atomic>)
#ifndef _WIN32
#include <time.h>
#endif

#include <stddef.h>

#include "port.h"
#include "win_types.h"

const uint8  kuint8max  = (( uint8) 0xFF);
const uint16 kuint16max = ((uint16) 0xFFFF);
const uint32 kuint32max = ((uint32) 0xFFFFFFFF);
const uint64 kuint64max = ((uint64) GG_LONGLONG(0xFFFFFFFFFFFFFFFF));
const  int8  kint8min   = ((  int8) 0x80);
const  int8  kint8max   = ((  int8) 0x7F);
const  int16 kint16min  = (( int16) 0x8000);
const  int16 kint16max  = (( int16) 0x7FFF);
const  int32 kint32min  = (( int32) 0x80000000);
const  int32 kint32max  = (( int32) 0x7FFFFFFF);
const  int64 kint64min  = ((  int64) GG_LONGLONG(0x8000000000000000));
const  int64 kint64max  = ((  int64) GG_LONGLONG(0x7FFFFFFFFFFFFFFF));

// DISALLOW_COPY_AND_ASSIGN prevents pay-by-value semantics and
// requires using the private: section for access.
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName&); \
    void operator=(const TypeName&)

// DISALLOW_IMPLICIT_CONSTRUCTORS prevents accidental sem-default constructors,
// copy constructors and pay-by-value assignment operators.
//
// Requires using the private: section to prevent external use, though a single
// static method is fine.
#define DISALLOW_IMPLICIT_CONSTRUCTORS(TypeName) \
    TypeName(); \
    DISALLOW_COPY_AND_ASSIGN(TypeName)

// ArraySizeHelper is a helper function that returns a reference to a char array
// of the correct size. It needs no implementation, as sizeof is all that's needed.
template<typename T, size_t N>
char (&ArraySizeHelper(T (&array)[N]))[N];

// arraysize(array) returns the number of elements in a static array.
// This assert-like method uses a compile-time constant so it won't:
//     a) require a runtime call
//     b) incorrectly accept a pointer (arraysize(ptr) will cause a compile-time error).
//     c) be fooled by sizeof(arr) being misleading on platforms where sizeof(bool)==1.
//
// arraysize() should only be used in code that is compile-time audited. For non-audit
// code, use the safer ARRAYSIZE_UNSAFE().
#define arraysize(array) (sizeof(ArraySizeHelper(array)))

// ARRAYSIZE_UNSAFE is like arraysize, except it is more relaxed - it can
// accept a pointer. Although it will cause a compile-time error if you pass
// it something dangerous.
//
// ARRAYSIZE_UNSAFE(a) is equivalent to a expression of type size_t.
//
// ARRAYSIZE_UNSAFE should not be used on anything but a static array.
//     "warning: division by zero in ..."
// If you get this warning, it's because you passed a non-array, and the
// 0/(sizeof(*(a))) "caught" it.
//
// ARRAYSIZE_UNSAFE(a) works by doing a compile-time division of (a) by
// sizeof(*(a)). If sizeof(a) is not a multiple of sizeof(*(a)),
// the compilation will error.
//
// Additionally, this approach will catch some dangerous edge cases
// (pointer arguments, const arguments, etc.), though not all.
//
// On 32-bit platforms, a pointer is 4 bytes, and anything with a pointer
// argument of 3 or fewer bytes in size may cause a false "division by zero"
// warning on some compilers.
#define ARRAYSIZE_UNSAFE(a) \
    ((sizeof(a)/sizeof(*(a))) / \
    static_cast<size_t>(!(sizeof(a)%sizeof(*(a)))))

// COMPILE_ASSERT is a compile-time assertion for verifying static assumptions.
// Use this to verify compile-time constants:
//     COMPILE_ASSERT(ARRAYSIZE_UNSAFE(content_type_names)==CONTENT_NUM_TYPES,
//         content_type_names_incorrect_size);
// Or to verify structure sizes:
template<bool>
struct CompileAssert {};

#undef COMPILE_ASSERT
#define COMPILE_ASSERT(expr, msg) \
    typedef CompileAssert<(bool(expr))> msg[bool(expr)?1:-1]

// bit_cast<Dest,Source> is a template function that implements "*reinterpret_cast<Dest*>(&source)"
// The same functionality.  This template is a utility for implementers
//     float f = 3.14159265358979;
//     int i = bit_cast<int32>(f);
//     // i = 0x40490fdb
//
// requiring DEST's size == SOURCE's size:
//
// The requirement that the two types have equal sizes is critical.
//     int i = * reinterpret_cast<int*>(&f);  // WRONG
//
// The "wrong" code above is actually correct in terms of behavior on
// some platforms, but it is not valid C++ and may not compile or run correctly
// on all platforms or with all compilers.
//
// C++ standard 3.10 -15 - indicates that a reinterpret_cast removes const-ness
// if and only if you are casting away const-ness. Therefore, using
// *(int*)&f is incorrect because it removes const-ness from f, even though
// f is const. The proper way is:
//     int i = * const_cast<int*>(&f);
//
// bit_cast<> should be used to ensure proper behavior, but one caveat:
// the behavior of memcpy() is implementation-defined when the source
// and destination are not POD. Although not guaranteed, most implementations
// do a simple byte copy. However, this is not guaranteed and should not
// be relied upon. Use bit_cast<> for well-defined semantics.
template<class Dest, class Source>
inline Dest bit_cast(const Source& source)
{
    // Compile-time assertion: sizeof(Dest) == sizeof(Source)
    // A compile-time error will occur if Dest and Source are not the same size.
    typedef char VerifySizesAreEqual[sizeof(Dest)==sizeof(Source) ? 1 : -1];

    Dest dest;
    memcpy(&dest, &source, sizeof(dest));
    return dest;
}

// LinkerInitialized is an enum that should only be used as a parameter to
// the constructor of a class, not for static storage. The reason for this
// is that LinkerInitialized could theoretically have any value, and it's
// better to be safe. If you want a static to start at 0, don't use
// LINKER_INITIALIZED and let it zero-initialize itself.
//
// A good example of when to use this:
//
//     explicit MyClass(base::LinkerInitialized x) {}
//
// And then use it like:
//
//     static MyClass my_variable_name(base::LINKER_INITIALIZED);
namespace base
{
    enum LinkerInitialized { LINKER_INITIALIZED };
} //namespace base

#endif //__basic_types_h__

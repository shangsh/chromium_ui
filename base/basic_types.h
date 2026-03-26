
#ifndef __basic_types_h__
#define __basic_types_h__

#pragma once

#include <stddef.h>

#include "port.h"
#include "win_types.h"

typedef signed char         schar;
typedef signed char         int8;
typedef short               int16;
typedef int                 int32;
#if __LP64__
typedef long                int64;
#else
typedef long long           int64;
#endif
typedef unsigned char       uint8;
typedef unsigned short      uint16;
typedef unsigned int        uint32;
#if __LP64__
typedef unsigned long       uint64;
#else
typedef unsigned long long  uint64;
#endif
typedef signed int          char32;

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
const  int64 kint64min  = (( int64) GG_LONGLONG(0x8000000000000000));
const  int64 kint64max  = (( int64) GG_LONGLONG(0x7FFFFFFFFFFFFFFF));

// DISALLOW_COPY_AND_ASSIGN���ÿ����͸�ֵ���캯��.
// ��Ҫ�����private:���ʿ�������ʹ��.
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName&); \
    void operator=(const TypeName&)

// DISALLOW_IMPLICIT_CONSTRUCTORS��ֹ��ʽ�Ĺ��캯��, ����ȱʡ���캯����
// �������캯���͸�ֵ���캯��.
//
// ��Ҫ�����private:���ʿ�������ʹ���Է�ֹʵ����, ����ֻ�о�̬������
// ��ǳ�����.
#define DISALLOW_IMPLICIT_CONSTRUCTORS(TypeName) \
    TypeName(); \
    DISALLOW_COPY_AND_ASSIGN(TypeName)

// ArraySizeHelper��һ����������Ϊchar[N]�ĺ���,���β�����Ϊ T[N].
// ����û��Ҫʵ��, ��Ϊsizeofֻ��Ҫ����.
template<typename T, size_t N>
char (&ArraySizeHelper(T (&array)[N]))[N];

// arraysize(arr)����array�����Ԫ�ظ���. �ñ���ʽ�Ǳ���ʱ����,
// �������ڶ����µ�����. �������һ��ָ��ᱨ����ʱ����.
//
// ���в������arraysize(arr)��֧���������ͺ��ں����ж��������.
// ��������¾ͱ���ʹ�÷����Ͱ�ȫ��ARRAYSIZE_UNSAFE()��. ������
// C++ģ��������Ƶ�, �Ժ��ȡ��.
#define arraysize(array) (sizeof(ArraySizeHelper(array)))

// ARRAYSIZE_UNSAFE���ڵĹ�����arraysizeһ��, ���������������ͺ�
// �ں����ж��������. ��û��arraysize��ȫ, ��Ϊ���Խ���һЩָ��
// ����(����ȫ��), ��������Ӧ�þ���ʹ��arraysize.
//
// ARRAYSIZE_UNSAFE(a)����ʽ������Ϊsize_t�ı���ʱ����.
//
// ARRAYSIZE_UNSAFE���Բ�׽һЩ���ʹ���. ����㿴���������
//     "warning: division by zero in ..."
// ���������Ĵ��ݸ�ARRAYSIZE_UNSAFE��һ��ָ��. ARRAYSIZE_UNSAFE
// ֻӦ�����ھ�̬���������.
//
// ARRAYSIZE_UNSAFE(arr)ͨ���ȶ�sizeof(arr)��sizeof(*(arr))ʵ�ֵ�.
// ���ǰ�߱����߿ɷ�, arr������һ������, �̾�������Ԫ�ظ���; ����
// arr��������, �ᱨ����ʱ����.
//
// bool���͵Ĵ�С�Ǿ���ʵ�ֶ����, ������Ҫ��!(sizeof(a) & sizeof(*(a)))
// ǿת��size_t��ȷ�����ս����size_t.
//
// ����겢������, �����Ľ���һЩָ��(ָ���С��������Ԫ�ش�С).
// ��32λƽ̨��, ָ���С��4�ֽ�, С��3���ߴ���4�ֽڵ�ָ�����Ͷ��ᱨ��.
#define ARRAYSIZE_UNSAFE(a) \
    ((sizeof(a)/sizeof(*(a))) / \
    static_cast<size_t>(!(sizeof(a)%sizeof(*(a)))))

// COMPILE_ASSERT�������ڱ���ʱ���Ա���ʽ. �������������֤��̬�����С:
//     COMPILE_ASSERT(ARRAYSIZE_UNSAFE(content_type_names)==CONTENT_NUM_TYPES,
//         content_type_names_incorrect_size);
//
// ����ȷ���ṹ��С��һ����С:
//     COMPILE_ASSERT(sizeof(foo)<128, foo_too_large);
// �ڶ���������Ǳ�����, �������ʽΪfalse, �����������һ�������������Ĵ���/����.
template<bool>
struct CompileAssert {};

// COMPILE_ASSERTʵ��ϸ��:
//
// - COMPILE_ASSERTͨ������һ������Ϊ-1������(�Ƿ�)��ʵ�ֵ�, ��ʱ����ʽfalse.
//
// - ����򻯵Ķ���
//       #define COMPILE_ASSERT(expr, msg) typedef char msg[(expr)?1:-1]
//   �ǷǷ���. ����gcc֧������ʱȷ�����ȵı䳤����(gcc��չ, ������C++��׼),
//   ����������μ򵥵Ĵ��붨�岻����:
//       int foo;
//       COMPILE_ASSERT(foo, msg); // not supposed to compile as foo is
//                                 // not a compile-time constant.
//
// - Ҫʹ������CompileAssert<(bool(expr))>, ����ȷ��expr�Ǳ���ʱ����.
//   (ģ������ڱ���ʱȷ��.)
//
// - CompileAssert<(bool(expr))>������Բ�������ڽ��gcc 3.4.4��4.0.1��
//   һ��bug. ���д��
//       CompileAssert<bool(expr)>
//   ���������޷�����
//       COMPILE_ASSERT(5>0, some_message);
//   ("5>0"�е�">"������Ϊ��ģ������б���β��">".)
//
// - �����С��(bool(expr)?1:-1)������((expr)?1:-1), ���Խ��MS VC 7.1
//   �а�((0.0)?1:-1)�������Ϊ1��bug.
#undef COMPILE_ASSERT
#define COMPILE_ASSERT(expr, msg) \
    typedef CompileAssert<(bool(expr))> msg[bool(expr)?1:-1]

// bit_cast<Dest,Source>ģ�庯��ʵ��"*reinterpret_cast<Dest*>(&source)"
// ��ͬ�Ĺ���. ��protobuf��Ϳ����㷨֧�ֵĵײ������ʹ��.
//
//     float f = 3.14159265358979;
//     int i = bit_cast<int32>(f);
//     // i = 0x40490fdb
//
// ����ĵ�ַǿ�Ʒ���:
//
//     // WRONG
//     float f = 3.14159265358979;            // WRONG
//     int i = * reinterpret_cast<int*>(&f);  // WRONG
//
// ����ISO C++�淶��3.10 -15 -��, �������������δ������Ϊ. ������,
// ����˵�����������ʹ�ò�ͬ�����ͷ���һ��������ڴ��ַ, �󲿷�
// �»ᵼ��δ������Ϊ.
//
// ����˵������*(int*)&f����*reinterpret_cast<int*>(&f)���ǳ�����,
// ������������ֵ�͸�������ֵ����ת��ʱ.
//
// 3.10 -15-Ŀ����Ϊ�������������Բ�ͬ���͵��ڴ����ñ���ʽ�����Ż�.
// gcc 4.0.1��������һ�Ż�. ���Բ��淶�ĳ�����ܻ������������.
//
// ���ⲻ����ʹ����reinterpret_cast, �������͵�˫����: �ڴ��еĶ�������
// �Ͷ�ȡ�ֽ�ʱ�����Ͳ�һ��.
//
// C++��׼�Ƿ��ȸ��ӵ�.
//
// Ȼ��...
//
// ϣ��bit_cast<>ʹ��memcpy()�����ر�׼, ������3.9�ڵ�ʾ��.
// ��Ȼbit_cast<>�Ѳ��õ��߼���װ������һ���ط�.
//
// memcpy()�Ƿǳ����. ���Ż�ģʽ��, gcc 2.95.3��gcc 4.0.1�ǳ������Ӷ�,
// msvc 7.1�����ɵĴ������ݰ�������. ��32λƽ̨, memcpy(d,s,4)����һ��
// ��ȡ����, memcpy(d,s,8)����2�δ�ȡ����.
//
// ʹ��gcc 2.95.3��gcc 4.0.1��icc 8.1��and msvc 7.1���Թ�����.
//
// ����: ���Dest����Source�Ƿ�POD����, memcpy�Ľ��������Ծ�.
template<class Dest, class Source>
inline Dest bit_cast(const Source& source)
{
    // ����ʱ����: sizeof(Dest) == sizeof(Source)
    // ���������ζ��Dest��Source��С��һ��.
    typedef char VerifySizesAreEqual[sizeof(Dest)==sizeof(Source) ? 1 : -1];

    Dest dest;
    memcpy(&dest, &source, sizeof(dest));
    return dest;
}

// LinkerInitializedö��ֻӦ��������Ĺ��캯������, �����Ǿ�̬�洢��.
// ���߶�������һ����̬ʵ���ǺϷ���, ����LINKER_INITIALIZED����.
// һ����й������������������������̬�����ǲ���ȫ��, ��Ϊʵ������
// �����ǲ�ȷ����. ���һ���������0����ʼ��, �������������������ڴ�,
// ��û���麯��, ���Ĺ��캯��������������:
//     explicit MyClass(base::LinkerInitialized x) {}
// �������µ���:
//     static MyClass my_variable_name(base::LINKER_INITIALIZED);
namespace base
{
    enum LinkerInitialized { LINKER_INITIALIZED };
} //namespace base

#endif //__basic_types_h__
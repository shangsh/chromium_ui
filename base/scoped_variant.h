
#ifndef __scoped_variant_h__
#define __scoped_variant_h__

#pragma once

#include <windows.h>
#include <oleauto.h>

#include "basic_types.h"

namespace base
{

    // ScopedVariant�����˳�������ʱ�Զ��ͷ�COM VARIANT. ����֮�⻹�ṩ��һЩ
    // ����ʹ��VARIANT�ķ�װ����.
    // ���ð��ݶ����������ķ�ʽ, �����и���Ŀ���Ȩ, �����ڴ�й¶.
    class ScopedVariant
    {
    public:
        // ����һ��ȫ�ֵ�VT_EMPTY���͵�variant����.
        static const VARIANT kEmptyVariant;

        // ȱʡ���캯��.
        ScopedVariant()
        {
            // �ȼ���VariantInit, ���Ǵ������.
            var_.vt = VT_EMPTY;
        }

        // ���캯��, ����VT_BSTR���͵�VARIANT.
        // ע��: ���ݵ�BSTR����Ȩ���ᱻ�ӹ�.
        explicit ScopedVariant(const wchar_t* str);

        // ����һ��ָ�����ȵ�VT_BSTR���͵�VARIANT.
        explicit ScopedVariant(const wchar_t* str, UINT length);

        // ����һ���������͵�VARIANT, ��ֵ��VARIANT.lVal(����32bit).
        explicit ScopedVariant(int value, VARTYPE vt=VT_I4);

        // ����һ��double�������͵�VARIANT, |vt|Ҫô��VT_R8Ҫô��VT_DATE.
        explicit ScopedVariant(double value, VARTYPE vt=VT_R8);

        // VT_DISPATCH.
        explicit ScopedVariant(IDispatch* dispatch);

        // VT_UNKNOWN.
        explicit ScopedVariant(IUnknown* unknown);

        // SAFEARRAY.
        explicit ScopedVariant(SAFEARRAY* safearray);

        // �������캯��.
        explicit ScopedVariant(const VARIANT& var);

        ~ScopedVariant();

        inline VARTYPE type() const
        {
            return var_.vt;
        }

        // ���½ӹ�һ���Է����VARIANT.
        void Reset(const VARIANT& var=kEmptyVariant);

        // �ͷ�VARIANT������Ȩ��������.
        VARIANT Release();

        // ��������ScopedVariant.
        void Swap(ScopedVariant& var);

        // ����һ��VARIANT����.
        VARIANT Copy() const;

        // ����0��ʾ���, 1��ʾ����|var|, -1��ʾС��|var|.
        int Compare(const VARIANT& var, bool ignore_case=false) const;

        // ����ָ���ַ.
        // ��������VARIANT���͵��������(�ӹ�����Ȩ). ������DCHECKs��ǰֵΪ
        // empty/null. �÷�: GetVariant(var.receive());
        VARIANT* Receive();

        void Set(const wchar_t* str);

        void Set(int8 i8);
        void Set(uint8 ui8);
        void Set(int16 i16);
        void Set(uint16 ui16);
        void Set(int32 i32);
        void Set(uint32 ui32);
        void Set(int64 i64);
        void Set(uint64 ui64);
        void Set(float r32);
        void Set(double r64);
        void Set(bool b);

        // ����|var|�Ŀ�����Ϊ�����ֵ. ע���ⲻͬ��Reset()����, Reset()
        // ���ͷŵ�ǰ���󲢽ӹ�����Ȩ.
        void Set(const VARIANT& var);

        // ֧��COM����.
        void Set(IDispatch* disp);
        void Set(IUnknown* unk);

        // ֧��SAFEARRAY.
        void Set(SAFEARRAY* array);

        // ֧��DATE, DATE��double. �Ѿ���һ��Set(double)��.
        void SetDate(DATE date);

        // ��������ʽ�����ڲ���variant, ����DCHECKs. ��������Ϊ��֧��V_XYZ
        // (����V_BSTR)ϵ�к�, ���ǲ������޸�, ��Ϊ������variant�Ŀ���Ȩ.
        const VARIANT* operator&() const
        {
            return &var_;
        }

        // ��������������(scoped_refptr��ScopedComPtr��ScopedBstr)һ��֧�ָ�ֵ������.
        ScopedVariant& operator=(const VARIANT& var);

        // ����ָ���һ������ֻ�����͵ĺ���, ���Ǻ���ԭ�����Ƿ�const��ָ��.
        // ���ﲻ��ҪDCHECK. ��������Ҫ����÷�.
        VARIANT* AsInput() const
        {
            return const_cast<VARIANT*>(&var_);
        }

        // ����ScopedVariantʵ����ֵ���߰��������ô���.
        operator const VARIANT&() const
        {
            return var_;
        }

        // ���ڵ���ʱ����Ƿ����ڴ�й©.
        static bool IsLeakableVarType(VARTYPE vt);

    protected:
        VARIANT var_;

    private:
        // ��֧��ScopedVariant�ıȽϲ���. ʹ��Compare�������.
        bool operator==(const ScopedVariant& var) const;
        bool operator!=(const ScopedVariant& var) const;
        DISALLOW_COPY_AND_ASSIGN(ScopedVariant);
    };

} //namespace base

#endif //__scoped_variant_h__
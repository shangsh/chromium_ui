
#ifndef __thread_local_storage_h__
#define __thread_local_storage_h__

#pragma once

#include "basic_types.h"

// �ֲ߳̾��洢��װ, �����ṩAPI����ֲ���ⲻ���κ���������.
class ThreadLocalStorage
{
public:
    // TLS��������ԭ��, �����߳��˳�ʱ����TLS, 'value'�Ǵ洢��TLS������.
    typedef void (*TLSDestructorFunc)(void* value);

    // ��ʾTLS��ȡ���ݵ�key.
    class Slot
    {
    public:
        explicit Slot(TLSDestructorFunc destructor=NULL);

        // Ӧ�þ�̬����, ����δ��ʼ���Ĳ��.
        explicit Slot(base::LinkerInitialized x) {}

        // ����TLS���. �ɹ��캯������. 'destructor'��һ������ָ��, ����
        // �߳��������. �������NULL, TLS��۲���Ҫ�������. ���󷵻�false.
        bool Initialize(TLSDestructorFunc destructor);

        // �ͷ���ǰ�����TLS���. ��������ٺ���, �Ƴ����ٺ���
        // �Ա�ʣ���߳��˳�ʱ�����ͷ�����.
        void Free();

        // ��ȡ�洢��TLS����е�ֵ. ��ʼ��Ϊ0.
        void* Get() const;

        // �洢ֵ'value'��TLS�����.
        void Set(void* value);

        bool initialized() const { return initialized_; }

    private:
        bool initialized_;
        int slot_;

        DISALLOW_COPY_AND_ASSIGN(Slot);
    };

    // �߳��˳�ʱ���ñ�����, �������е����ٺ���. �ڲ�ʹ��.
    static void ThreadExit();

private:
    // �ӳٳ�ʼ��TLS�ĺ���.
    static void** Initialize();

private:
    // TLS��۵����ջֵ, Ŀǰ�ǹ̶���. �ɾ�̬������, Ҳ�������ɶ�̬��.
    static const int kThreadLocalStorageSize = 64;

    static long tls_key_;
    static long tls_max_;
    static TLSDestructorFunc tls_destructors_[kThreadLocalStorageSize];

    DISALLOW_COPY_AND_ASSIGN(ThreadLocalStorage);
};

#endif //__thread_local_storage_h__

#ifndef __scoped_ptr_h__
#define __scoped_ptr_h__

#pragma once

#include <assert.h>
#include <cstddef>
#include <stdlib.h>

// Scopers�ܰ��������ָ�������Ȩ, �����������ʱ�Զ�����ά����ָ��.
// ��������ֱ��Ӧnew/delete��new[]/delete[]����.
//
// �÷�ʾ��(scoped_ptr):
//     {
//       scoped_ptr<Foo> foo(new Foo("wee"));
//     } // foo goes out of scope, releasing the pointer with it.
//
//     {
//       scoped_ptr<Foo> foo;          // No pointer managed.
//       foo.reset(new Foo("wee"));    // Now a pointer is managed.
//       foo.reset(new Foo("wee2"));   // Foo("wee") was destroyed.
//       foo.reset(new Foo("wee3"));   // Foo("wee2") was destroyed.
//       foo->Method();                // Foo::Method() called.
//       foo.get()->Method();          // Foo::Method() called.
//       SomeFunc(foo.release());      // SomeFunc takes ownership, foo no longer
//                                     // manages a pointer.
//       foo.reset(new Foo("wee4"));   // foo manages a pointer again.
//       foo.reset();                  // Foo("wee4") destroyed, foo no longer
//                                     // manages a pointer.
//     } // foo wasn't managing a pointer, so nothing was destroyed.
//
// �÷�ʾ��(scoped_array):
//     {
//       scoped_array<Foo> foo(new Foo[100]);
//       foo.get()->Method();  // Foo::Method on the 0th element.
//       foo[10].Method();     // Foo::Method on the 10th element.
//     }


// scoped_ptr<T>��T*����, ֻ�ǻ���������ʱ���Զ�����ά����ָ��.
// Ҳ����˵, scoped_ptr<T>ӵ��T���������Ȩ.
// ��T*һ��, scoped_ptr<T>�ڲ���ָ�����ΪNULL����ָ��T����, ���̰߳�ȫ,
// һ��������, �̰߳�ȫ�Ծ�������T����.
//
// scoped_ptr�����С: sizeof(scoped_ptr<C>) == sizeof(C*).
template<class C>
class scoped_ptr
{
public:
    // Ԫ������.
    typedef C element_type;

    // ���캯��. ȱʡ��NULL��ʼ��, �����ܴ���һ��δ��ʼ����scoped_ptr.
    // �����������ʹ��new����.
    explicit scoped_ptr(C* p=NULL) : ptr_(p) {}

    // ��������, ɾ���ڲ��Ķ���. �����ж�ptr_==NULL, C++�Լ��ᴦ��.
    ~scoped_ptr()
    {
        enum { type_must_be_complete = sizeof(C) };
        delete ptr_;
    }

    // ����. ɾ�����еĶ���, �ӹ��µĶ�������Ȩ.
    void reset(C* p=NULL)
    {
        if(p != ptr_)
        {
            enum { type_must_be_complete = sizeof(C) };
            delete ptr_;
            ptr_ = p;
        }
    }

    // ����ӵ�еĶ���.
    // operator*��operator->��û�ж����ʱ����������.
    C& operator*() const
    {
        assert(ptr_ != NULL);
        return *ptr_;
    }
    C* operator->() const
    {
        assert(ptr_ != NULL);
        return ptr_;
    }
    C* get() const { return ptr_; }

    // �Ƚϲ���.
    // �ж�scoped_ptr���õĶ����Ƿ�Ϊp, �������������������.
    bool operator==(C* p) const { return ptr_ == p; }
    bool operator!=(C* p) const { return ptr_ != p; }

    // ����ָ��.
    void swap(scoped_ptr& p2)
    {
        C* tmp = ptr_;
        ptr_ = p2.ptr_;
        p2.ptr_ = tmp;
    }

    // �ͷ�ָ��.
    // ���ص�ǰӵ�еĶ���ָ��, ���Ϊ���򷵻�NULL.
    // �������֮��ӵ�п�ָ��, ����ӵ���κζ���.
    C* release()
    {
        C* retVal = ptr_;
        ptr_ = NULL;
        return retVal;
    }

private:
    C* ptr_;

    // ��ֹ��scoped_ptr���ͱȽ�. C2!=C�ıȽ�û���κ�����, C2==CҲûʲô������Ϊ
    // �㲻��������������ӵ����ͬ��ָ��.
    template<class C2> bool operator==(scoped_ptr<C2> const& p2) const;
    template<class C2> bool operator!=(scoped_ptr<C2> const& p2) const;

    scoped_ptr(const scoped_ptr&);
    void operator=(const scoped_ptr&);
};

template<class C>
void swap(scoped_ptr<C>& p1, scoped_ptr<C>& p2)
{
    p1.swap(p2);
}

template<class C>
bool operator==(C* p1, const scoped_ptr<C>& p2)
{
    return p1 == p2.get();
}

template<class C>
bool operator!=(C* p1, const scoped_ptr<C>& p2)
{
    return p1 != p2.get();
}

// scoped_array<C>��scoped_ptr<C>����, �������߱���ʹ��new[]�������, ����
// ����ʱ�����delete[]ɾ������.
//
// scoped_array<C>Ҫôָ��һ������Ҫôָ��NULL, ӵ��ָ�������Ȩ, �̰߳�ȫ.
// һ������ָ��, ���ض�����̰߳�ȫ������T����.
//
// ��С: sizeof(scoped_array<C>) == sizeof(C*).
template<class C>
class scoped_array
{
public:
    // Ԫ������.
    typedef C element_type;

    // ���캯��. ȱʡ��NULL��ʼ��, �����ܴ���һ��δ��ʼ����scoped_array.
    // �����������ʹ��new[]����.
    explicit scoped_array(C* p = NULL) : array_(p) { }

    // ��������, ɾ���ڲ��Ķ���. �����ж�ptr_==NULL, C++�Լ��ᴦ��.
    ~scoped_array()
    {
        enum { type_must_be_complete = sizeof(C) };
        delete[] array_;
    }

    // ����. ɾ�����еĶ���, �ӹ��µĶ�������Ȩ.
    // this->reset(this->get())����������.
    void reset(C* p = NULL)
    {
        if(p != array_)
        {
            enum { type_must_be_complete = sizeof(C) };
            delete[] array_;
            array_ = p;
        }
    }

    // ��ȡָ��ָ�����������.
    // ���ָ��Ϊ�ջ�������Ϊ�������������.
    C& operator[](std::ptrdiff_t i) const
    {
        assert(i >= 0);
        assert(array_ != NULL);
        return array_[i];
    }

    // ��ȡָ�����.
    C* get() const
    {
        return array_;
    }

    // �Ƚϲ���.
    // �ж�scoped_array���õĶ����Ƿ�Ϊp, �������������������.
    bool operator==(C* p) const { return array_ == p; }
    bool operator!=(C* p) const { return array_ != p; }

    // ����ָ��.
    void swap(scoped_array& p2)
    {
        C* tmp = array_;
        array_ = p2.array_;
        p2.array_ = tmp;
    }

    // �ͷ�����ָ��.
    // ���ص�ǰӵ�еĶ���ָ��, ���Ϊ���򷵻�NULL.
    // �������֮��ӵ�п�ָ��, ����ӵ���κζ���.
    C* release()
    {
        C* retVal = array_;
        array_ = NULL;
        return retVal;
    }

private:
    C* array_;

    // ��ֹ��scoped_array���ͱȽ�.
    template<class C2> bool operator==(scoped_array<C2> const& p2) const;
    template<class C2> bool operator!=(scoped_array<C2> const& p2) const;

    scoped_array(const scoped_array&);
    void operator=(const scoped_array&);
};

template<class C>
void swap(scoped_array<C>& p1, scoped_array<C>& p2)
{
    p1.swap(p2);
}

template<class C>
bool operator==(C* p1, const scoped_array<C>& p2)
{
    return p1 == p2.get();
}

template<class C>
bool operator!=(C* p1, const scoped_array<C>& p2)
{
    return p1 != p2.get();
}

// ���װC�������free(), ����scoped_ptr_malloc��ģ�����
class ScopedPtrMallocFree
{
public:
    inline void operator()(void* x) const
    {
        free(x);
    }
};

// scoped_ptr_malloc<>����scoped_ptr<>, ���ܵڶ���ģ����������ͷŶ���.
template<class C, class FreeProc=ScopedPtrMallocFree>
class scoped_ptr_malloc
{
public:
    // Ԫ������.
    typedef C element_type;

    // ���캯��. ȱʡ��NULL��ʼ��, �����ܴ���һ��δ��ʼ����scoped_ptr_malloc.
    // �������ķ�����������ͷź�����ƥ��. ����ȱʡ���ͷź���, ���亯������
    // ��malloc��calloc��realloc.
    explicit scoped_ptr_malloc(C* p=NULL): ptr_(p) {}

    // ��������. �����C����, ����free����.
    ~scoped_ptr_malloc()
    {
        free_(ptr_);
    }

    // ����. ɾ�����еĶ���, �ӹ��µĶ�������Ȩ.
    // this->reset(this->get())����������.
    void reset(C* p=NULL)
    {
        if(ptr_ != p)
        {
            free_(ptr_);
            ptr_ = p;
        }
    }

    // ����ӵ�еĶ���.
    // operator*��operator->��û�ж����ʱ����������.
    C& operator*() const
    {
        assert(ptr_ != NULL);
        return *ptr_;
    }

    C* operator->() const
    {
        assert(ptr_ != NULL);
        return ptr_;
    }

    C* get() const
    {
        return ptr_;
    }

    // �Ƚϲ���.
    // �ж�scoped_ptr_malloc���õĶ����Ƿ�Ϊp, �������������������.
    // Ϊ�˱��ֺ�boostʵ�ֵļ���, �����Ƿ�const����.
    bool operator==(C* p) const
    {
        return ptr_ == p;
    }

    bool operator!=(C* p) const
    {
        return ptr_ != p;
    }

    // ����ָ��.
    void swap(scoped_ptr_malloc & b)
    {
        C* tmp = b.ptr_;
        b.ptr_ = ptr_;
        ptr_ = tmp;
    }

    // �ͷ�ָ��.
    // ���ص�ǰӵ�еĶ���ָ��, ���Ϊ���򷵻�NULL.
    // �������֮��ӵ�п�ָ��, ����ӵ���κζ���.
    C* release()
    {
        C* tmp = ptr_;
        ptr_ = NULL;
        return tmp;
    }

private:
    C* ptr_;

    template<class C2, class GP>
    bool operator==(scoped_ptr_malloc<C2, GP> const& p) const;
    template<class C2, class GP>
    bool operator!=(scoped_ptr_malloc<C2, GP> const& p) const;

    static FreeProc const free_;

    scoped_ptr_malloc(const scoped_ptr_malloc&);
    void operator=(const scoped_ptr_malloc&);
};

template<class C, class FP>
FP const scoped_ptr_malloc<C, FP>::free_ = FP();

template<class C, class FP> inline
void swap(scoped_ptr_malloc<C, FP>& a, scoped_ptr_malloc<C, FP>& b)
{
    a.swap(b);
}

template<class C, class FP> inline
bool operator==(C* p, const scoped_ptr_malloc<C, FP>& b)
{
    return p == b.get();
}

template<class C, class FP> inline
bool operator!=(C* p, const scoped_ptr_malloc<C, FP>& b)
{
    return p != b.get();
}

#endif //__scoped_ptr_h__
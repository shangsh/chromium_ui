
#ifndef __stack_container_h__
#define __stack_container_h__

#pragma once

#include <string>
#include <vector>

#include "basic_types.h"

// ��ΪSTL�����ṩջ�����������ڴ�ķ�����, ���ʱ�ڶ��Ϸ���. ջ��������
// ���ڴ������һ���̶��ϱ���ѷ���.
//
// STLϲ������������, ��˷����������ܱ���ʵ������. �������������𴴽�
// StackAllocator::Source�洢����. ����������ֻ�ǹ���һ��sourceָ��, ����
// ����StackAllocator�����ķ�����������һ��ջ������.
//
// ջ��������ʵ�ֺܼ�. ��һ�η���ʱ���ջ�ռ�������ʹ��ջ������, ����
// ���κη��䶼����ʹ��ջ������, ��ʹ��ʣ��ռ�. ���ʺ����������������,
// ����������Ҫreserve()������С��ջ��������С, ������������һ����С����
// �����"�ù�"ջ������.
template<typename T, size_t stack_capacity>
class StackAllocator : public std::allocator<T>
{
public:
    typedef typename std::allocator<T>::pointer pointer;
    typedef typename std::allocator<T>::size_type size_type;

    // ������֧�ִ洢. ��������ά���������.
    struct Source
    {
        Source() : used_stack_buffer_(false) {}

        // ǿת����ȷ����.
        T* stack_buffer() { return reinterpret_cast<T*>(stack_buffer_); }
        const T* stack_buffer() const
        {
            return reinterpret_cast<const T*>(stack_buffer_);
        }

        // ��Ҫ: ��ر�֤stack_buffer_�Ƕ����, ��Ϊ��ģ��һ������ΪT
        // ������. ��stack_buffer_ǰ�������κηǶ�������(��bool)����ҪС��.

        // ������. ����������T, ��Ϊ���ǲ��빹�캯���������������Զ�����.
        // ����һ����С��ȵ�POD�������滻.
        char stack_buffer_[sizeof(T[stack_capacity])];

        // ջ������������ʹ��ʱ����. ����¼������ʹ���˶���, ֻ��¼�Ƿ���
        // ����ʹ��.
        bool used_stack_buffer_;
    };

    // ��������Ҫ����һ������ΪU�ķ�����ʱʹ��.
    template<typename U>
    struct rebind
    {
        typedef StackAllocator<U, stack_capacity> other;
    };

    // ֱ��ʹ�ÿ������캯��, ���Թ���洢.
    StackAllocator(const StackAllocator<T, stack_capacity>& rhs)
        : std::allocator<T>(), source_(rhs.source_) {}

    // ISO C++Ҫ��������Ĺ��캯��, ���������, VC++2008SP1 Release
    // �е�std::vector������_Container_base_aux_alloc_real(����
    // <xutility>)����.
    // ����������캯��, ���ǲ��ܹ���洢, ��Ϊ�޷���֤Source�Ŀռ�һ
    // ����U�Ĵ�.
    // TODO: �����Ҫ����, ��sizeof(T)==sizeof(U)ʱ������洢.
    template<typename U, size_t other_capacity>
    StackAllocator(const StackAllocator<U, other_capacity>& other)
        : source_(NULL) {}

    explicit StackAllocator(Source* source) : source_(source) {}

    // ʵ�ʷ��乤��. ʹ��ջ������, ����ռ仹û��ʹ���Ҵ�С�㹻. �����ɱ�׼
    // ��׼����������.
    pointer allocate(size_type n, void* hint=0)
    {
        if(source_!=NULL && !source_->used_stack_buffer_
            && n<=stack_capacity)
        {
            source_->used_stack_buffer_ = true;
            return source_->stack_buffer();
        }
        else
        {
            return std::allocator<T>::allocate(n, hint);
        }
    }

    // �ͷ�: �����ͷ�ջ������ʱ, ֻ�ñ����û�ü���. ���ڷ�ջ������ָ��, ����
    // ��׼����������.
    void deallocate(pointer p, size_type n)
    {
        if(source_!=NULL && p==source_->stack_buffer())
        {
            source_->used_stack_buffer_ = false;
        }
        else
        {
            std::allocator<T>::deallocate(p, n);
        }
    }

private:
    Source* source_;
};

// ����STL�����ķ�װ, ά��һ��ջ������, ��С���������ĳ�ʼ����. ������������ջ
// ��������Сʱ���Զ��ڶ��Ϸ����ڴ�. ��������֧��reserve().
//
// С��: ContainerType����ʹ����ȷ��StackAllocator����. �������Ҫ���ڲ�ʹ��.
// ��ʹ������ķ�װ��.
template<typename TContainerType, int stack_capacity>
class StackContainer
{
public:
    typedef TContainerType ContainerType;
    typedef typename ContainerType::value_type ContainedType;
    typedef StackAllocator<ContainedType, stack_capacity> Allocator;

    // Allocator��������container����!
    StackContainer() : allocator_(&stack_data_), container_(allocator_)
    {
        // �����в���֮ǰ, ����ͨ��Ԥ��stack_capacityʹ��ջ����.
        container_.reserve(stack_capacity);
    }

    // ��ȡʵ�ʵ�����.
    //
    // Σ��: ʹ�÷���ֵ������������Ķ����������ڱ����Դ�����. ���������
    // ����ͬһ������, ����ջ����������һ��. �����Ҫ�������ڸ����Ķ���, ʹ
    // ��std::copy���������������.
    ContainerType& container() { return container_; }
    const ContainerType& container() const { return container_; }

    // ֧��->������ȡ����:
    //     StackContainer<...> foo;
    //     std::sort(foo->begin(), foo->end());
    ContainerType* operator->() { return &container_; }
    const ContainerType* operator->() const { return &container_; }

protected:
    typename Allocator::Source stack_data_;
    Allocator allocator_;
    ContainerType container_;

    DISALLOW_COPY_AND_ASSIGN(StackContainer);
};

// StackString
template<size_t stack_capacity>
class StackString : public StackContainer<
    std::basic_string<char,
    std::char_traits<char>,
    StackAllocator<char, stack_capacity> >,
    stack_capacity>
{
public:
    StackString() : StackContainer<
        std::basic_string<char,
        std::char_traits<char>,
        StackAllocator<char, stack_capacity> >,
        stack_capacity>() {}

private:
    DISALLOW_COPY_AND_ASSIGN(StackString);
};

// StackWString
template<size_t stack_capacity>
class StackWString : public StackContainer<
    std::basic_string<wchar_t,
    std::char_traits<wchar_t>,
    StackAllocator<wchar_t, stack_capacity> >,
    stack_capacity>
{
public:
    StackWString() : StackContainer<
        std::basic_string<wchar_t,
        std::char_traits<wchar_t>,
        StackAllocator<wchar_t, stack_capacity> >,
        stack_capacity>() {}

private:
    DISALLOW_COPY_AND_ASSIGN(StackWString);
};

// StackVector
//
// ʾ��:
//     StackVector<int, 16> foo;
//     foo->push_back(22);  // ������->������.
//     foo[0] = 10;         // ������[]������.
template<typename T, size_t stack_capacity>
class StackVector : public StackContainer<
    std::vector<T, StackAllocator<T, stack_capacity> >,
    stack_capacity>
{
public:
    StackVector() : StackContainer<
        std::vector<T, StackAllocator<T, stack_capacity> >,
        stack_capacity>() {}

    // ��ʱ������һЩ��ҪԪ���п������캯����STL������. ���ܵ��ó���Ŀ�������
    // ����, ��Ϊ������ȡ��Դ�����ջ������. ����, ����һ���ն���, �����Լ���
    // ջ������.
    StackVector(const StackVector<T, stack_capacity>& other)
        : StackContainer<
        std::vector<T, StackAllocator<T, stack_capacity> >,
        stack_capacity>()
    {
        this->container().assign(other->begin(), other->end());
    }

    StackVector<T, stack_capacity>& operator=(
        const StackVector<T, stack_capacity>& other)
    {
        this->container().assign(other->begin(), other->end());
        return *this;
    }

    T& operator[](size_t i) { return this->container().operator[](i); }
    const T& operator[](size_t i) const
    {
        return this->container().operator[](i);
    }
};

#endif //__stack_container_h__
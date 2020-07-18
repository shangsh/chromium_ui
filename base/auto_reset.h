
#ifndef __auto_reset_h__
#define __auto_reset_h__

#pragma once

#include "basic_types.h"

// AutoResetValue�����趨�������ض�������Χ�ڵ�ֵ. �ٸ�����, ������
// �������ڵ���"var = false;"����"var = old_var;", ���Կ���ʹ�������.
//
// ���ܼܺ�, ����Ҫע��AutoResetValueʵ�����������ڱ�scoped_variable��,
// ����AutoResetValue�뿪������ʱд�Ƿ��ڴ�.
template<typename T>
class AutoReset
{
public:
    AutoReset(T* scoped_variable, T new_value)
        : scoped_variable_(scoped_variable),
        original_value_(*scoped_variable)
    {
        *scoped_variable_ = new_value;
    }

    ~AutoReset() { *scoped_variable_ = original_value_; }

private:
    T* scoped_variable_;
    T original_value_;

    DISALLOW_COPY_AND_ASSIGN(AutoReset);
};

#endif //__auto_reset_h__
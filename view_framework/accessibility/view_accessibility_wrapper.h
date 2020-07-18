
#ifndef __view_accessibility_wrapper_h__
#define __view_accessibility_wrapper_h__

#pragma once

#include <oleacc.h>

#include "base/scoped_comptr.h"

namespace view
{
    class View;
}

////////////////////////////////////////////////////////////////////////////////
//
// ViewAccessibilityWrapper
//
// 封装视图可访问接口平台相关的指针封装.
//
////////////////////////////////////////////////////////////////////////////////
class ViewAccessibilityWrapper
{
public:
    explicit ViewAccessibilityWrapper(view::View* view);
    ~ViewAccessibilityWrapper() {}

    STDMETHODIMP CreateDefaultInstance(REFIID iid);

    HRESULT Uninitialize();

    // 返回对象的指定接口. 如果已经存在会复用, 否则会创建一个新的指针.
    STDMETHODIMP GetInstance(REFIID iid, void** interface_ptr);

    // 设置用户指定的可访问接口实现.
    STDMETHODIMP SetInstance(IAccessible* interface_ptr);

private:
    // 可访问信息实例, 存储在View中.
    base::ScopedComPtr<IAccessible> accessibility_info_;

    // 初始化IAccessible需要的视图.
    view::View* view_;

    DISALLOW_COPY_AND_ASSIGN(ViewAccessibilityWrapper);
};

#endif //__view_accessibility_wrapper_h__

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
// ��װ��ͼ�ɷ��ʽӿ�ƽ̨��ص�ָ���װ.
//
////////////////////////////////////////////////////////////////////////////////
class ViewAccessibilityWrapper
{
public:
    explicit ViewAccessibilityWrapper(view::View* view);
    ~ViewAccessibilityWrapper() {}

    STDMETHODIMP CreateDefaultInstance(REFIID iid);

    HRESULT Uninitialize();

    // ���ض����ָ���ӿ�. ����Ѿ����ڻḴ��, ����ᴴ��һ���µ�ָ��.
    STDMETHODIMP GetInstance(REFIID iid, void** interface_ptr);

    // �����û�ָ���Ŀɷ��ʽӿ�ʵ��.
    STDMETHODIMP SetInstance(IAccessible* interface_ptr);

private:
    // �ɷ�����Ϣʵ��, �洢��View��.
    base::ScopedComPtr<IAccessible> accessibility_info_;

    // ��ʼ��IAccessible��Ҫ����ͼ.
    view::View* view_;

    DISALLOW_COPY_AND_ASSIGN(ViewAccessibilityWrapper);
};

#endif //__view_accessibility_wrapper_h__
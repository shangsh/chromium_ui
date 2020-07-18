
#include "view_accessibility_wrapper.h"

#include "base/scoped_variant.h"

#include "view_accessibility.h"

ViewAccessibilityWrapper::ViewAccessibilityWrapper(view::View* view)
: accessibility_info_(NULL), view_(view) {}

STDMETHODIMP ViewAccessibilityWrapper::CreateDefaultInstance(REFIID iid)
{
    if(IID_IUnknown==iid || IID_IDispatch==iid || IID_IAccessible==iid)
    {
        // �����û�д���ViewAccessibilityʵ��, �½�һ��. ������֮ǰ��ʵ��.
        if(!accessibility_info_)
        {
            CComObject<ViewAccessibility>* instance = NULL;

            HRESULT hr = CComObject<ViewAccessibility>::CreateInstance(&instance);

            if(!SUCCEEDED(hr) || !instance)
            {
                return E_FAIL;
            }

            base::ScopedComPtr<IAccessible> accessibility_instance(instance);

            if(!SUCCEEDED(instance->Initialize(view_)))
            {
                return E_FAIL;
            }

            // ��ʱ����ֵ���������ָ��
            accessibility_info_.Attach(accessibility_instance.Detach());
        }
        return S_OK;
    }
    // ��֧�ֵĽӿ�.
    return E_NOINTERFACE;
}

HRESULT ViewAccessibilityWrapper::Uninitialize()
{
    view_ = NULL;
    if(accessibility_info_.get())
    {
        accessibility_info_->put_accValue(
            base::ScopedVariant(kViewUninitializeAccessibilityInstance), NULL);
        ::CoDisconnectObject(accessibility_info_.get(), 0);
        accessibility_info_ = NULL;
    }

    return S_OK;
}

STDMETHODIMP ViewAccessibilityWrapper::GetInstance(REFIID iid,
                                                   void** interface_ptr)
{
    if(IID_IUnknown==iid || IID_IDispatch==iid || IID_IAccessible==iid)
    {
        // �����û�д���������ʵ��, �����½�һ��ȱʡ��. ������֮ǰ��ʵ��.
        if(!accessibility_info_)
        {
            HRESULT hr = CreateDefaultInstance(iid);

            if(hr != S_OK)
            {
                // ʵ������ʧ��.
                *interface_ptr = NULL;
                return E_NOINTERFACE;
            }
        }
        *interface_ptr = static_cast<IAccessible*>(accessibility_info_);
        return S_OK;
    }
    // ��֧�ֵĽӿ�, ���ش���.
    *interface_ptr = NULL;
    return E_NOINTERFACE;
}

STDMETHODIMP ViewAccessibilityWrapper::SetInstance(IAccessible* interface_ptr)
{
    if(!interface_ptr)
    {
        return E_NOINTERFACE;
    }

    accessibility_info_.Attach(interface_ptr);

    // ƫִ�ļ��, ȷ���洢��һ���Ϸ���IAccessibleָ��.
    if(!accessibility_info_)
    {
        return E_FAIL;
    }

    return S_OK;
}
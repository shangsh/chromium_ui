
#include "view_accessibility_wrapper.h"

#include "base/scoped_variant.h"

#include "view_accessibility.h"

ViewAccessibilityWrapper::ViewAccessibilityWrapper(view::View* view)
: accessibility_info_(NULL), view_(view) {}

STDMETHODIMP ViewAccessibilityWrapper::CreateDefaultInstance(REFIID iid)
{
    if(IID_IUnknown==iid || IID_IDispatch==iid || IID_IAccessible==iid)
    {
        // 如果还没有创建ViewAccessibility实例, 新建一个. 否则复用之前的实例.
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

            // 临时对象赋值到类的智能指针
            accessibility_info_.Attach(accessibility_instance.Detach());
        }
        return S_OK;
    }
    // 不支持的接口.
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
        // 如果还没有创建访问性实例, 现在新建一个缺省的. 否则复用之前的实例.
        if(!accessibility_info_)
        {
            HRESULT hr = CreateDefaultInstance(iid);

            if(hr != S_OK)
            {
                // 实例创建失败.
                *interface_ptr = NULL;
                return E_NOINTERFACE;
            }
        }
        *interface_ptr = static_cast<IAccessible*>(accessibility_info_);
        return S_OK;
    }
    // 不支持的接口, 返回错误.
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

    // 偏执的检查, 确保存储了一个合法的IAccessible指针.
    if(!accessibility_info_)
    {
        return E_FAIL;
    }

    return S_OK;
}
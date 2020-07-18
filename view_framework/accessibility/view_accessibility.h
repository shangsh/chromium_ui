
#ifndef __view_accessibility_h__
#define __view_accessibility_h__

#pragma once

#include <atlbase.h>
#include <atlcom.h>

#include <oleacc.h>

#include "../controls/native_view_host.h"
#include "../view.h"

////////////////////////////////////////////////////////////////////////////////
//
// ViewAccessibility
//
// ΪViewʵ��MSAA IAccessible COM�ӿ�, Ϊ�ṩ��Ļ�Ķ��߻����������������ṩ
// ������֧��.
//
////////////////////////////////////////////////////////////////////////////////
class ATL_NO_VTABLE ViewAccessibility
    : public CComObjectRootEx<CComMultiThreadModel>,
    public IDispatchImpl<IAccessible, &IID_IAccessible, &LIBID_Accessibility>
{
public:
    BEGIN_COM_MAP(ViewAccessibility)
        COM_INTERFACE_ENTRY2(IDispatch, IAccessible)
        COM_INTERFACE_ENTRY(IAccessible)
    END_COM_MAP()

    ViewAccessibility() {}
    ~ViewAccessibility() {}

    HRESULT Initialize(view::View* view);

    // ֧�ֵ�IAccessible����.

    // ������Ļ�ϸ��������Ԫ�ػ����Ӷ���.
    STDMETHODIMP accHitTest(LONG x_left, LONG y_top, VARIANT* child);

    // ����ָ������ĵ�ǰ��Ļλ��.
    STDMETHODIMP accLocation(LONG* x_left,
        LONG* y_top,
        LONG* width,
        LONG* height,
        VARIANT var_id);

    // ����������UIԪ�ز�����.
    STDMETHODIMP accNavigate(LONG nav_dir, VARIANT start, VARIANT* end);

    // ����ָ�����ӵ�IDispatch�ӿ�ָ��.
    STDMETHODIMP get_accChild(VARIANT var_child, IDispatch** disp_child);

    // ���ؿɷ��ʵĺ�����Ŀ.
    STDMETHODIMP get_accChildCount(LONG* child_count);

    // ���ض���ȱʡ�������ַ�������.
    STDMETHODIMP get_accDefaultAction(VARIANT var_id, BSTR* default_action);

    // ������ʾ��Ϣ.
    STDMETHODIMP get_accDescription(VARIANT var_id, BSTR* desc);

    // ���ض����Ƿ��м��̽���.
    STDMETHODIMP get_accFocus(VARIANT* focus_child);

    // ����ָ������Ŀ�ݼ�.
    STDMETHODIMP get_accKeyboardShortcut(VARIANT var_id, BSTR* access_key);

    // ����ָ�����������.
    STDMETHODIMP get_accName(VARIANT var_id, BSTR* name);

    // ���ض����׵�IDispatch�ӿ�ָ��.
    STDMETHODIMP get_accParent(IDispatch** disp_parent);

    // ����ָ������Ľ�ɫ������Ϣ.
    STDMETHODIMP get_accRole(VARIANT var_id, VARIANT* role);

    // ����ָ������ĵ�ǰ״̬.
    STDMETHODIMP get_accState(VARIANT var_id, VARIANT* state);

    // ����ָ������ĵ�ǰֵ.
    STDMETHODIMP get_accValue(VARIANT var_id, BSTR* value);

    // ��֧�ֵ�IAccessible����.

    // ��ʱ��, ����˵�����õ�.
    STDMETHODIMP accDoDefaultAction(VARIANT var_id);

    // ��ͼ��ѡ���ǲ���Ӧ�õ�.
    STDMETHODIMP get_accSelection(VARIANT* selected);
    STDMETHODIMP accSelect(LONG flags_sel, VARIANT var_id);

    // ��֧�ְ�������.
    STDMETHODIMP get_accHelp(VARIANT var_id, BSTR* help);
    STDMETHODIMP get_accHelpTopic(BSTR* help_file,
        VARIANT var_id, LONG* topic_id);

    // �����Ĺ���, ���ﲻʵ��.
    STDMETHODIMP put_accName(VARIANT var_id, BSTR put_name);
    STDMETHODIMP put_accValue(VARIANT var_id, BSTR put_val);

    // �¼�(accessibility_types.h�ж����)ת����MSAA�¼�, ������.
    static int32 MSAAEvent(AccessibilityTypes::Event event);

    // ��ɫ(accessibility_types.h�ж����)ת����MSAA��ɫ, ������.
    static int32 MSAARole(AccessibilityTypes::Role role);

    // ״̬(accessibility_types.h�ж����)ת����MSAA״̬, ������.
    static int32 MSAAState(AccessibilityTypes::State state);

private:
    // �ж�accNavigate�ĵ�������, ���Ϻ�ǰӳ��Ϊǰ, �ҡ��ºͺ�ӳ��Ϊ��.
    // ������������Ǻ󷵻�true, ���򷵻�false.
    bool IsNavDirNext(int nav_dir) const;

    // �жϵ���Ŀ���Ƿ�������Χ. ������򷵻�true, ���򷵻�false.
    bool IsValidNav(int nav_dir,
        int start_id,
        int lower_bound,
        int upper_bound) const;

    // �ж�child�Ƿ�Ϸ�.
    bool IsValidId(const VARIANT& child) const;

    // ����view��IAccessibleʵ����Wrapper.
    ViewAccessibilityWrapper* GetViewAccessibilityWrapper(view::View* v) const
    {
        return v->GetViewAccessibilityWrapper();
    }

    // ������ͼ��Ӧ��״̬�ĸ�������.
    void SetState(VARIANT* msaa_state, view::View* view);

    // ���ر�����ͼ��IAccessible�ӿ�(����ǿ�Ӧ�õ�). �ɹ�����S_OK.
    HRESULT GetNativeIAccessibleInterface(view::NativeViewHost* native_host,
        IDispatch** disp_child);

    HRESULT GetNativeIAccessibleInterface(HWND native_view_window,
        IDispatch** disp_child);

    // ��ͼ��Ա.
    view::View* view_;

    DISALLOW_COPY_AND_ASSIGN(ViewAccessibility);
};

extern const wchar_t kViewUninitializeAccessibilityInstance[];
extern const wchar_t kViewNativeHostPropForAccessibility[];

#endif //__view_accessibility_h__
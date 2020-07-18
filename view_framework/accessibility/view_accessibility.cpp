
#include "view_accessibility.h"

#include "view_accessibility_wrapper.h"
#include "../widget/widget.h"
#include "../widget/widget_win.h"

const wchar_t kViewUninitializeAccessibilityInstance[] =
L"View_Uninitialize_AccessibilityInstance";
const wchar_t kViewNativeHostPropForAccessibility[] =
L"View_NativeViewHostHWNDForAccessibility";

HRESULT ViewAccessibility::Initialize(view::View* view)
{
    if(!view)
    {
        return E_INVALIDARG;
    }

    view_ = view;
    return S_OK;
}

// TODO: 处理子视图不被父视图包含的情况.
STDMETHODIMP ViewAccessibility::accHitTest(
    LONG x_left, LONG y_top, VARIANT* child)
{
    if(!child)
    {
        return E_INVALIDARG;
    }

    if(!view_)
    {
        return E_FAIL;
    }

    gfx::Point point(x_left, y_top);
    view::View::ConvertPointToView(NULL, view_, &point);

    if(!view_->HitTest(point))
    {
        // 如果视图本身没被击中, 返回失败.
        child->vt = VT_EMPTY;
        return S_FALSE;
    }

    view::View* view = view_->GetViewForPoint(point);
    if(view == view_)
    {
        // 没有击中孩子视图, 返回父id.
        child->vt = VT_I4;
        child->lVal = CHILDID_SELF;
    }
    else
    {
        child->vt = VT_DISPATCH;
        if((GetViewAccessibilityWrapper(view))->GetInstance(IID_IAccessible,
            reinterpret_cast<void**>(&child->pdispVal)) == S_OK)
        {
            // 对返回的接口引用计数.
            child->pdispVal->AddRef();
            return S_OK;
        }
        else
        {
            return E_NOINTERFACE;
        }
    }

    return S_OK;
}

STDMETHODIMP ViewAccessibility::accLocation(LONG* x_left, LONG* y_top,
                                            LONG* width, LONG* height,
                                            VARIANT var_id)
{
    if(!IsValidId(var_id) || !x_left || !y_top || !width || !height)
    {
        return E_INVALIDARG;
    }

    if(!view_)
    {
        return E_FAIL;
    }

    gfx::Rect view_bounds;
    // 返回父视图用于从视图到屏幕坐标转换.
    view::View* parent = view_->GetParent();

    if(parent == NULL)
    {
        // 如果没有父亲, 父亲设置为自己.
        parent = view_;
    }

    // 返回当前视图的范围.
    view_bounds = view_->bounds();

    if(!view_bounds.IsEmpty())
    {
        *width  = view_bounds.width();
        *height = view_bounds.height();

        gfx::Point topleft(view_bounds.origin());
        view::View::ConvertPointToScreen(parent, &topleft);
        *x_left = topleft.x();
        *y_top  = topleft.y();
    }
    else
    {
        return E_FAIL;
    }
    return S_OK;
}

STDMETHODIMP ViewAccessibility::accNavigate(LONG nav_dir, VARIANT start,
                                            VARIANT* end)
{
    if(start.vt!=VT_I4 || !end)
    {
        return E_INVALIDARG;
    }

    if(!view_)
    {
        return E_FAIL;
    }

    switch(nav_dir)
    {
    case NAVDIR_FIRSTCHILD:
    case NAVDIR_LASTCHILD:
        {
            if(start.lVal != CHILDID_SELF)
            {
                // 导航的起始必须是View自己.
                return E_INVALIDARG;
            }
            else if(view_->GetChildViewCount() == 0)
            {
                // 没找到孩子.
                return S_FALSE;
            }

            // 根据起始和结束位置设置child_id.
            int child_id = 0;
            if(nav_dir == NAVDIR_LASTCHILD)
            {
                child_id = view_->GetChildViewCount() - 1;
            }

            view::View* child = view_->GetChildViewAt(child_id);

            if(child->GetChildViewCount() != 0)
            {
                end->vt = VT_DISPATCH;
                if((GetViewAccessibilityWrapper(child))->GetInstance(IID_IAccessible,
                    reinterpret_cast<void**>(&end->pdispVal)) == S_OK)
                {
                    // 对返回的接口引用计数.
                    end->pdispVal->AddRef();
                    return S_OK;
                }
                else
                {
                    return E_NOINTERFACE;
                }
            }
            else
            {
                end->vt = VT_I4;
                // 设置返回孩子的lVal, 根据MSAA索引规则(MSAA的孩子索引从1开始,
                // 而视图的索引从0开始)进行调整.
                end->lVal = child_id + 1;
            }
            break;
        }
    case NAVDIR_LEFT:
    case NAVDIR_UP:
    case NAVDIR_PREVIOUS:
    case NAVDIR_RIGHT:
    case NAVDIR_DOWN:
    case NAVDIR_NEXT:
        {
            // 返回父亲用于访问视图索引和执行边界检查.
            view::View* parent = view_->GetParent();
            if(!parent)
            {
                return E_FAIL;
            }

            if(start.lVal == CHILDID_SELF)
            {
                int view_index = parent->GetChildIndex(view_);
                // 检查导航边界, 调整视图的孩子索引(MSAA的孩子索引从1开始, 而视图
                // 的索引从0开始).
                if(!IsValidNav(nav_dir, view_index, -1,
                    parent->GetChildViewCount()-1))
                {
                    // 导航试图越界.
                    end->vt = VT_EMPTY;
                    return S_FALSE;
                }
                else
                {
                    if(IsNavDirNext(nav_dir))
                    {
                        view_index += 1;
                    }
                    else
                    {
                        view_index -=1;
                    }
                }

                view::View* child = parent->GetChildViewAt(view_index);
                if(child->GetChildViewCount() != 0)
                {
                    end->vt = VT_DISPATCH;
                    // 返回非叶子孩子的IDispatch.
                    if((GetViewAccessibilityWrapper(child))->GetInstance(IID_IAccessible,
                        reinterpret_cast<void**>(&end->pdispVal)) == S_OK)
                    {
                        // 对返回的接口引用计数.
                        end->pdispVal->AddRef();
                        return S_OK;
                    }
                    else
                    {
                        return E_NOINTERFACE;
                    }
                }
                else
                {
                    end->vt = VT_I4;
                    // 根据MSAA的规则修改视图的lVal(MSAA的孩子索引从1开始, 而视图的索引
                    // 从0开始).
                    end->lVal = view_index + 1;
                }
            }
            else
            {
                // 检查导航边界, 调整孩子索引(MSAA的孩子索引从1开始, 而视图的索引从0
                // 开始).
                if(!IsValidNav(nav_dir, start.lVal, 0,
                    parent->GetChildViewCount()+1))
                {
                    // 导航试图越界.
                    end->vt = VT_EMPTY;
                    return S_FALSE;
                }
                else
                {
                    if(IsNavDirNext(nav_dir))
                    {
                        start.lVal += 1;
                    }
                    else
                    {
                        start.lVal -= 1;
                    }
                }

                HRESULT result = this->get_accChild(start, &end->pdispVal);
                if(result == S_FALSE)
                {
                    // 孩子是叶子.
                    end->vt = VT_I4;
                    end->lVal = start.lVal;
                }
                else if(result == E_INVALIDARG)
                {
                    return E_INVALIDARG;
                }
                else
                {
                    // 孩子不是叶子.
                    end->vt = VT_DISPATCH;
                }
            }
            break;
        }
    default:
        return E_INVALIDARG;
    }
    // 导航正确执行. 函数的全局返回, 如果有错误发生会提前退出.
    return S_OK;
}

STDMETHODIMP ViewAccessibility::get_accChild(VARIANT var_child,
                                             IDispatch** disp_child)
{
    if(var_child.vt!=VT_I4 || !disp_child)
    {
        return E_INVALIDARG;
    }

    if(!view_)
    {
        return E_FAIL;
    }

    LONG child_id = V_I4(&var_child);

    if(child_id == CHILDID_SELF)
    {
        // 还是相同的dispatch.
        return S_OK;
    }

    view::View* child_view = NULL;
    if(child_id > 0)
    {
        if(child_id <= view_->GetChildViewCount())
        {
            // 注意: 对于孩子索引, child_id是从1开始的.
            child_view = view_->GetChildViewAt(child_id - 1);
        }
        else
        {
            // 尝试返回给定id的孩子视图.
            child_view = view_->GetViewByID(child_id);
        }
    }
    else
    {
        // view的WidgetWin触发的事件使用负值.
        view::WidgetWin* widget =
            static_cast<view::WidgetWin*>(view_->GetWidget());
        child_view = widget->GetAccessibilityViewEventAt(child_id);
    }

    if(!child_view)
    {
        // 没找到孩子.
        *disp_child = NULL;
        return E_FAIL;
    }

    // 首先检查孩子视图是否为本地视图.
    if(child_view->GetClassName() == view::NativeViewHost::kViewClassName)
    {
        view::NativeViewHost* native_host =
            static_cast<view::NativeViewHost*>(child_view);
        if(GetNativeIAccessibleInterface(native_host, disp_child) == S_OK)
        {
            return S_OK;
        }
    }

    // 接着检查孩子视图是否为widget容器.
    if(child_view->child_widget())
    {
        view::WidgetWin* native_widget =
            reinterpret_cast<view::WidgetWin*>(child_view->child_widget());
        if(GetNativeIAccessibleInterface(native_widget->GetNativeView(),
            disp_child) == S_OK)
        {
            return S_OK;
        }
    }

    // 最后尝试我们的ViewAccessibility实现. 查询孩子视图的IUnknown
    // 接口, 赋值给IDispatch.
    HRESULT hr = GetViewAccessibilityWrapper(child_view)->
        GetInstance(IID_IAccessible, reinterpret_cast<void**>(disp_child));
    if(hr == S_OK)
    {
        // 对返回的接口引用计数.
        (*disp_child)->AddRef();
        return S_OK;
    }
    else
    {
        return E_NOINTERFACE;
    }
}

STDMETHODIMP ViewAccessibility::get_accChildCount(LONG* child_count)
{
    if(!child_count || !view_)
    {
        return E_INVALIDARG;
    }

    if(!view_)
    {
        return E_FAIL;
    }

    *child_count = view_->GetChildViewCount();
    return S_OK;
}

STDMETHODIMP ViewAccessibility::get_accDefaultAction(
    VARIANT var_id, BSTR* def_action)
{
    if(!IsValidId(var_id) || !def_action)
    {
        return E_INVALIDARG;
    }

    if(!view_)
    {
        return E_FAIL;
    }

    std::wstring temp_action = view_->GetAccessibleDefaultAction();

    if(!temp_action.empty())
    {
        *def_action = SysAllocString(temp_action.c_str());
    }
    else
    {
        return S_FALSE;
    }

    return S_OK;
}

STDMETHODIMP ViewAccessibility::get_accDescription(VARIANT var_id, BSTR* desc)
{
    if(!IsValidId(var_id) || !desc)
    {
        return E_INVALIDARG;
    }

    std::wstring temp_desc;

    view_->GetTooltipText(gfx::Point(), &temp_desc);
    if(!temp_desc.empty())
    {
        *desc = SysAllocString(temp_desc.c_str());
    }
    else
    {
        return S_FALSE;
    }

    return S_OK;
}

STDMETHODIMP ViewAccessibility::get_accFocus(VARIANT* focus_child)
{
    if(!focus_child)
    {
        return E_INVALIDARG;
    }

    if(!view_)
    {
        return E_FAIL;
    }

    if(view_->GetChildViewCount()==0 && view_->HasFocus())
    {
        // 父视图有焦点.
        focus_child->vt = VT_I4;
        focus_child->lVal = CHILDID_SELF;
    }
    else
    {
        bool has_focus = false;
        int child_count = view_->GetChildViewCount();
        // 查找有焦点的孩子视图.
        for(int child_id=0; child_id<child_count; ++child_id)
        {
            if(view_->GetChildViewAt(child_id)->HasFocus())
            {
                focus_child->vt = VT_I4;
                focus_child->lVal = child_id + 1;

                // 如果孩子视图不是叶子, 返回IDispatch.
                if(view_->GetChildViewAt(child_id)->GetChildViewCount() != 0)
                {
                    focus_child->vt = VT_DISPATCH;
                    this->get_accChild(*focus_child, &focus_child->pdispVal);
                }
                has_focus = true;
                break;
            }
        }
        // 所有孩子当前都没有焦点.
        if(!has_focus)
        {
            focus_child->vt = VT_EMPTY;
            return S_FALSE;
        }
    }

    return S_OK;
}

STDMETHODIMP ViewAccessibility::get_accKeyboardShortcut(
    VARIANT var_id, BSTR* acc_key)
{
    if(!IsValidId(var_id) || !acc_key)
    {
        return E_INVALIDARG;
    }

    if(!view_)
    {
        return E_FAIL;
    }

    std::wstring temp_key = view_->GetAccessibleKeyboardShortcut();

    if(!temp_key.empty())
    {
        *acc_key = SysAllocString(temp_key.c_str());
    }
    else
    {
        return S_FALSE;
    }

    return S_OK;
}

STDMETHODIMP ViewAccessibility::get_accName(VARIANT var_id, BSTR* name)
{
    if(!IsValidId(var_id) || !name)
    {
        return E_INVALIDARG;
    }

    if(!view_)
    {
        return E_FAIL;
    }

    std::wstring temp_name;

    // 返回当前视图的名字.
    view_->GetAccessibleName(&temp_name);
    if(!temp_name.empty())
    {
        // 返回名字.
        *name = SysAllocString(temp_name.c_str());
    }
    else
    {
        // 如果没有名字返回S_FALSE.
        return S_FALSE;
    }

    return S_OK;
}

STDMETHODIMP ViewAccessibility::get_accParent(IDispatch** disp_parent)
{
    if(!disp_parent)
    {
        return E_INVALIDARG;
    }

    if(!view_)
    {
        return E_FAIL;
    }

    view::View* parent_view = view_->GetParent();

    if(!parent_view)
    {
        // 函数可能在WidetWin关闭的时候调用, 所以获取HWND失败就直接退出.
        if(!view_->GetWidget() || !view_->GetWidget()->GetNativeView())
        {
            *disp_parent = NULL;
            return S_FALSE;
        }

        // 对于没有父亲的视图(比如根视图), 父指针指向缺省实现、Windows层接口和所有
        // 支持WindowFromAccessibleObject等调用的对象.
        HRESULT hr = ::AccessibleObjectFromWindow(
            view_->GetWidget()->GetNativeView(),
            OBJID_WINDOW, IID_IAccessible,
            reinterpret_cast<void**>(disp_parent));

        if(!SUCCEEDED(hr))
        {
            *disp_parent = NULL;
            return S_FALSE;
        }

        return S_OK;
    }

    // 返回父视图的IUnknown接口, 赋值到IDispatch并返回.
    if((GetViewAccessibilityWrapper(parent_view))->GetInstance(
        IID_IAccessible, reinterpret_cast<void**>(disp_parent)) == S_OK)
    {
        // 对返回的接口引用计数.
        (*disp_parent)->AddRef();
        return S_OK;
    }
    else
    {
        return E_NOINTERFACE;
    }
}

STDMETHODIMP ViewAccessibility::get_accRole(VARIANT var_id, VARIANT* role)
{
    if(!IsValidId(var_id) || !role)
    {
        return E_INVALIDARG;
    }

    role->vt = VT_I4;
    role->lVal = MSAARole(view_->GetAccessibleRole());
    return S_OK;
}

STDMETHODIMP ViewAccessibility::get_accState(VARIANT var_id, VARIANT* state)
{
    if(!IsValidId(var_id) || !state)
    {
        return E_INVALIDARG;
    }

    state->vt = VT_I4;

    // 返回当前所有可应用的状态.
    this->SetState(state, view_);

    // 确保状态不为空, 且有正确的类型.
    if(state->vt == VT_EMPTY)
    {
        return E_FAIL;
    }

    return S_OK;
}

STDMETHODIMP ViewAccessibility::get_accValue(VARIANT var_id, BSTR* value)
{
    if(!IsValidId(var_id) || !value)
    {
        return E_INVALIDARG;
    }

    if(!view_)
    {
        return E_FAIL;
    }

    // 返回当前视图的值.
    std::wstring temp_value = view_->GetAccessibleValue();

    if(!temp_value.empty())
    {
        // 返回值.
        *value = SysAllocString(temp_value.c_str());
    }
    else
    {
        // 如果视图没有值, 回退到缺省实现.
        *value = NULL;
        return E_NOTIMPL;
    }

    return S_OK;
}

bool ViewAccessibility::IsNavDirNext(int nav_dir) const
{
    if(nav_dir==NAVDIR_RIGHT || nav_dir==NAVDIR_DOWN || nav_dir==NAVDIR_NEXT)
    {
        return true;
    }
    return false;
}

bool ViewAccessibility::IsValidNav(int nav_dir, int start_id, int lower_bound,
                                   int upper_bound) const
{
    if(IsNavDirNext(nav_dir))
    {
        if((start_id+1) > upper_bound)
        {
            return false;
        }
    }
    else
    {
        if((start_id-1) <= lower_bound)
        {
            return false;
        }
    }
    return true;
}

bool ViewAccessibility::IsValidId(const VARIANT& child) const
{
    // 视图的可访问性会为每个视图返回IAccessible, 所以我们只支持CHILDID_SELF id.
    return (VT_I4==child.vt) && (CHILDID_SELF==child.lVal);
}

void ViewAccessibility::SetState(VARIANT* msaa_state, view::View* view)
{
    // 确保输出参数初始化为0.
    msaa_state->lVal = 0;

    // 缺省状态; 所有视图都具有可访问性焦点.
    msaa_state->lVal |= STATE_SYSTEM_FOCUSABLE;

    if(!view)
    {
        return;
    }

    if(!view->IsEnabled())
    {
        msaa_state->lVal |= STATE_SYSTEM_UNAVAILABLE;
    }
    if(!view->IsVisible())
    {
        msaa_state->lVal |= STATE_SYSTEM_INVISIBLE;
    }
    if(view->IsHotTracked())
    {
        msaa_state->lVal |= STATE_SYSTEM_HOTTRACKED;
    }
    if(view->IsPushed())
    {
        msaa_state->lVal |= STATE_SYSTEM_PRESSED;
    }
    // 同时检查实际视图焦点和可访问性焦点.
    view::View* parent = view->GetParent();

    if(view->HasFocus())
    {
        msaa_state->lVal |= STATE_SYSTEM_FOCUSED;
    }

    // 添加视图指定的状态.
    msaa_state->lVal |= MSAAState(view->GetAccessibleState());
}

// 不支持的IAccessible函数.

HRESULT ViewAccessibility::accDoDefaultAction(VARIANT var_id)
{
    return E_NOTIMPL;
}

STDMETHODIMP ViewAccessibility::get_accSelection(VARIANT* selected)
{
    if(selected)
    {
        selected->vt = VT_EMPTY;
    }
    return E_NOTIMPL;
}

STDMETHODIMP ViewAccessibility::accSelect(LONG flagsSelect, VARIANT var_id)
{
    return E_NOTIMPL;
}

STDMETHODIMP ViewAccessibility::get_accHelp(VARIANT var_id, BSTR* help)
{
    if(help)
    {
        *help = NULL;
    }
    return E_NOTIMPL;
}

STDMETHODIMP ViewAccessibility::get_accHelpTopic(
    BSTR* help_file, VARIANT var_id, LONG* topic_id)
{
    if(help_file)
    {
        *help_file = NULL;
    }
    if(topic_id)
    {
        *topic_id = static_cast<LONG>(-1);
    }
    return E_NOTIMPL;
}

STDMETHODIMP ViewAccessibility::put_accName(VARIANT var_id, BSTR put_name)
{
    // 废弃.
    return E_NOTIMPL;
}

STDMETHODIMP ViewAccessibility::put_accValue(VARIANT var_id, BSTR put_val)
{
    // TODO: 这种用法看起来是错误的. var_id应该是VT_I4类型.
    if(V_VT(&var_id) == VT_BSTR)
    {
        if(!lstrcmpi(var_id.bstrVal, kViewUninitializeAccessibilityInstance))
        {
            view_ = NULL;
            return S_OK;
        }
    }
    // 废弃.
    return E_NOTIMPL;
}

int32 ViewAccessibility::MSAAEvent(AccessibilityTypes::Event event)
{
    switch(event)
    {
    case AccessibilityTypes::EVENT_ALERT:
        return EVENT_SYSTEM_ALERT;
    case AccessibilityTypes::EVENT_FOCUS:
        return EVENT_OBJECT_FOCUS;
    case AccessibilityTypes::EVENT_MENUSTART:
        return EVENT_SYSTEM_MENUSTART;
    case AccessibilityTypes::EVENT_MENUEND:
        return EVENT_SYSTEM_MENUEND;
    case AccessibilityTypes::EVENT_MENUPOPUPSTART:
        return EVENT_SYSTEM_MENUPOPUPSTART;
    case AccessibilityTypes::EVENT_MENUPOPUPEND:
        return EVENT_SYSTEM_MENUPOPUPEND;
    default:
        // 不支持或者非法事件.
        NOTREACHED();
        return -1;
    }
}

int32 ViewAccessibility::MSAARole(AccessibilityTypes::Role role)
{
    switch(role)
    {
    case AccessibilityTypes::ROLE_ALERT:
        return ROLE_SYSTEM_ALERT;
    case AccessibilityTypes::ROLE_APPLICATION:
        return ROLE_SYSTEM_APPLICATION;
    case AccessibilityTypes::ROLE_BUTTONDROPDOWN:
        return ROLE_SYSTEM_BUTTONDROPDOWN;
    case AccessibilityTypes::ROLE_BUTTONMENU:
        return ROLE_SYSTEM_BUTTONMENU;
    case AccessibilityTypes::ROLE_CHECKBUTTON:
        return ROLE_SYSTEM_CHECKBUTTON;
    case AccessibilityTypes::ROLE_COMBOBOX:
        return ROLE_SYSTEM_COMBOBOX;
    case AccessibilityTypes::ROLE_DIALOG:
        return ROLE_SYSTEM_DIALOG;
    case AccessibilityTypes::ROLE_GRAPHIC:
        return ROLE_SYSTEM_GRAPHIC;
    case AccessibilityTypes::ROLE_GROUPING:
        return ROLE_SYSTEM_GROUPING;
    case AccessibilityTypes::ROLE_LINK:
        return ROLE_SYSTEM_LINK;
    case AccessibilityTypes::ROLE_MENUBAR:
        return ROLE_SYSTEM_MENUBAR;
    case AccessibilityTypes::ROLE_MENUITEM:
        return ROLE_SYSTEM_MENUITEM;
    case AccessibilityTypes::ROLE_MENUPOPUP:
        return ROLE_SYSTEM_MENUPOPUP;
    case AccessibilityTypes::ROLE_OUTLINE:
        return ROLE_SYSTEM_OUTLINE;
    case AccessibilityTypes::ROLE_OUTLINEITEM:
        return ROLE_SYSTEM_OUTLINEITEM;
    case AccessibilityTypes::ROLE_PAGETAB:
        return ROLE_SYSTEM_PAGETAB;
    case AccessibilityTypes::ROLE_PAGETABLIST:
        return ROLE_SYSTEM_PAGETABLIST;
    case AccessibilityTypes::ROLE_PANE:
        return ROLE_SYSTEM_PANE;
    case AccessibilityTypes::ROLE_PROGRESSBAR:
        return ROLE_SYSTEM_PROGRESSBAR;
    case AccessibilityTypes::ROLE_PUSHBUTTON:
        return ROLE_SYSTEM_PUSHBUTTON;
    case AccessibilityTypes::ROLE_RADIOBUTTON:
        return ROLE_SYSTEM_RADIOBUTTON;
    case AccessibilityTypes::ROLE_SCROLLBAR:
        return ROLE_SYSTEM_SCROLLBAR;
    case AccessibilityTypes::ROLE_SEPARATOR:
        return ROLE_SYSTEM_SEPARATOR;
    case AccessibilityTypes::ROLE_STATICTEXT:
        return ROLE_SYSTEM_STATICTEXT;
    case AccessibilityTypes::ROLE_TEXT:
        return ROLE_SYSTEM_TEXT;
    case AccessibilityTypes::ROLE_TITLEBAR:
        return ROLE_SYSTEM_TITLEBAR;
    case AccessibilityTypes::ROLE_TOOLBAR:
        return ROLE_SYSTEM_TOOLBAR;
    case AccessibilityTypes::ROLE_WINDOW:
        return ROLE_SYSTEM_WINDOW;
    case AccessibilityTypes::ROLE_CLIENT:
    default:
        // MSAA缺省的角色.
        return ROLE_SYSTEM_CLIENT;
    }
}

int32 ViewAccessibility::MSAAState(AccessibilityTypes::State state)
{
    int32 msaa_state = 0;
    if(state & AccessibilityTypes::STATE_CHECKED)
    {
        msaa_state |= STATE_SYSTEM_CHECKED;
    }
    if(state & AccessibilityTypes::STATE_COLLAPSED)
    {
        msaa_state |= STATE_SYSTEM_COLLAPSED;
    }
    if(state & AccessibilityTypes::STATE_DEFAULT)
    {
        msaa_state |= STATE_SYSTEM_DEFAULT;
    }
    if(state & AccessibilityTypes::STATE_EXPANDED)
    {
        msaa_state |= STATE_SYSTEM_EXPANDED;
    }
    if(state & AccessibilityTypes::STATE_HASPOPUP)
    {
        msaa_state |= STATE_SYSTEM_HASPOPUP;
    }
    if(state & AccessibilityTypes::STATE_HOTTRACKED)
    {
        msaa_state |= STATE_SYSTEM_HOTTRACKED;
    }
    if(state & AccessibilityTypes::STATE_INVISIBLE)
    {
        msaa_state |= STATE_SYSTEM_INVISIBLE;
    }
    if(state & AccessibilityTypes::STATE_LINKED)
    {
        msaa_state |= STATE_SYSTEM_LINKED;
    }
    if(state & AccessibilityTypes::STATE_OFFSCREEN)
    {
        msaa_state |= STATE_SYSTEM_OFFSCREEN;
    }
    if(state & AccessibilityTypes::STATE_PRESSED)
    {
        msaa_state |= STATE_SYSTEM_PRESSED;
    }
    if(state & AccessibilityTypes::STATE_PROTECTED)
    {
        msaa_state |= STATE_SYSTEM_PROTECTED;
    }
    if(state & AccessibilityTypes::STATE_READONLY)
    {
        msaa_state |= STATE_SYSTEM_READONLY;
    }
    if(state & AccessibilityTypes::STATE_SELECTED)
    {
        msaa_state |= STATE_SYSTEM_SELECTED;
    }
    if(state & AccessibilityTypes::STATE_FOCUSED)
    {
        msaa_state |= STATE_SYSTEM_FOCUSED;
    }
    if(state & AccessibilityTypes::STATE_UNAVAILABLE)
    {
        msaa_state |= STATE_SYSTEM_UNAVAILABLE;
    }
    return msaa_state;
}

HRESULT ViewAccessibility::GetNativeIAccessibleInterface(
    view::NativeViewHost* native_host, IDispatch** disp_child)
{
    if(!native_host || !disp_child)
    {
        return E_INVALIDARG;
    }

    HWND native_view_window =
        static_cast<HWND>(GetProp(native_host->native_view(),
        kViewNativeHostPropForAccessibility));
    if(!IsWindow(native_view_window))
    {
        native_view_window = native_host->native_view();
    }

    return GetNativeIAccessibleInterface(native_view_window, disp_child);
}

HRESULT ViewAccessibility::GetNativeIAccessibleInterface(
    HWND native_view_window , IDispatch** disp_child)
{
    if(IsWindow(native_view_window))
    {
        LRESULT ret = SendMessage(native_view_window, WM_GETOBJECT, 0,
            OBJID_CLIENT);
        return ObjectFromLresult(ret, IID_IDispatch, 0,
            reinterpret_cast<void**>(disp_child));
    }

    return E_FAIL;
}
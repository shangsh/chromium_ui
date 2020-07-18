
#include <tchar.h>
#include <windows.h>
#include <initguid.h>
#include <oleacc.h>

#include "gfx/gdiplus_initializer.h"
#include "view_framework/animation/bounds_animator.h"
#include "view_framework/app/resource_bundle.h"
#include "view_framework/controls/button/text_button.h"
#include "view_framework/controls/single_split_view.h"
#include "view_framework/focus/accelerator_handler.h"
#include "view_framework/window/dialog_delegate.h"
#include "view_framework/window/window_win.h"
#include "view_framework/widget/root_view.h"
#include "view_framework/layout/box_layout.h"
#include "view_framework/layout/grid_layout.h"

#include "resource.h"

#include <atlbase.h>

CComModule _Module;

#define BUTTON_ID_ANIMATE   1

class MainView : public view::View
{
public:
    virtual gfx::Size GetPreferredSize() { return gfx::Size(200, 200); }
};

class MainWindowDelegate : public view::WindowDelegate,
    public view::ButtonListener
{
    MainView* content_view_;
    scoped_ptr<view::BoundsAnimator> animator_;

public:
    MainWindowDelegate()
    {
        content_view_ = new MainView();
        view::GridLayout* layout = new view::GridLayout(content_view_);
        content_view_->set_background(
            view::Background::CreateSolidBackground(gfx::Color(214, 229, 247)));
        content_view_->SetAccessibleName(L"内容视图");
        content_view_->SetLayoutManager(layout);

        view::ColumnSet* column_set = layout->AddColumnSet(0);
        column_set->AddColumn(view::GridLayout::FILL, view::GridLayout::FILL,
            1, view::GridLayout::USE_PREF, 0, 0);
        layout->StartRow(0, 0);
        view::TextButton* button = new view::TextButton(NULL, L"带图标按钮");
        button->SetFocusable(true);
        button->SetIcon(ResourceBundle::GetSharedInstance().GetBitmapNamed(
            IDR_DEFAULT_FAVICON));
        layout->AddView(button);
        
        layout->StartRow(0, 0);
        button = new view::TextButton(NULL, L"文本居中对齐按钮");
        button->SetFocusable(true);
        button->set_alignment(view::TextButton::ALIGN_CENTER);
        layout->AddView(button);

        layout->StartRow(0, 0);
        button = new view::TextButton(NULL, L"文本右对齐按钮");
        button->SetFocusable(true);
        button->set_alignment(view::TextButton::ALIGN_RIGHT);
        layout->AddView(button);

        layout->StartRow(0, 0);
        view::View* v = new view::View();
        layout->AddView(v);
        v->SetLayoutManager(new view::BoxLayout(
            view::BoxLayout::kHorizontal, 0, 0, 0));
        button = new view::TextButton(NULL, L"按钮一");
        button->SetFocusable(true);
        v->AddChildView(button);
        button = new view::TextButton(NULL, L"按钮二");
        button->SetFocusable(true);
        v->AddChildView(button);
        button = new view::TextButton(NULL, L"按钮三");
        button->SetFocusable(true);
        v->AddChildView(button);

        layout->StartRow(1, 0);
        view::View* v1 = new view::View();
        view::View* v2 = new view::View();
        v1->set_background(view::Background::CreateStandardPanelBackground());
        v1->set_border(view::Border::CreateSolidBorder(1, gfx::Color(80, 80, 80)));
        v2->set_border(view::Border::CreateSolidBorder(1, gfx::Color(80, 80, 80)));
        v2->set_background(view::Background::CreateStandardPanelBackground());
        layout->AddView(new view::SingleSplitView(v1, v2,
            view::SingleSplitView::HORIZONTAL_SPLIT));

        layout->StartRow(1, 0);
        v = new view::View();
        layout->AddView(v);
        button = new view::TextButton(this, L"动画按钮");
        button->SetID(BUTTON_ID_ANIMATE);
        button->SetFocusable(true);
        button->SetIcon(ResourceBundle::GetSharedInstance().GetBitmapNamed(
            IDR_DEFAULT_FAVICON));
        button->set_alignment(view::TextButton::ALIGN_CENTER);
        button->SetBounds(100, 20, 100, 30);
        v->AddChildView(button);
    }

    ~MainWindowDelegate() {}

    virtual bool CanResize() const
    {
        return true;
    }

    virtual bool CanMaximize() const
    {
        return false;	
    }

    virtual std::wstring GetWindowTitle() const
    {
        return L"测试视图";
    }

    virtual void WindowClosing()
    {
        MessageLoopForUI::current()->Quit();
    }

    virtual view::View* GetContentsView()
    {
        return content_view_;
    }

    virtual void ButtonPressed(view::Button* sender, const view::Event& event)
    {
        if(sender->GetID()==BUTTON_ID_ANIMATE)
        {
            if(!animator_.get())
            {
                animator_.reset(new view::BoundsAnimator(sender->GetParent()));
            }

            if(animator_->IsAnimating())
            {
                return ;
            }

            gfx::Rect new_pos = sender->bounds();
            if(new_pos.x() == 100)
            {
                new_pos.Offset(100, 100);
                new_pos.set_width(new_pos.width()*2);
                new_pos.set_height(new_pos.height()*2);
            }
            else
            {
                new_pos.Offset(-100, -100);
                new_pos.set_width(new_pos.width()/2);
                new_pos.set_height(new_pos.height()/2);
            }

            DCHECK(animator_.get());
            animator_->AnimateViewTo(sender, new_pos);
        }
    }
};

class AppIdConveter : public ResourceBundle::IdConveter
{
public:
    int AppIdToResId(ResourceBundle::AppId app_id)
    {
        int resource_id = 0;

        switch(app_id)
        {
        case ResourceBundle::BITMAP_FRAME:
            resource_id = IDR_FRAME;
            break;
        case ResourceBundle::BITMAP_FRAME_INACTIVE:
            resource_id = IDR_FRAME_INACTIVE;
            break;
        case ResourceBundle::BITMAP_WINDOW_TOP_LEFT_CORNER:
            resource_id = IDR_WINDOW_TOP_LEFT_CORNER;
            break;
        case ResourceBundle::BITMAP_WINDOW_TOP_CENTER:
            resource_id = IDR_WINDOW_TOP_CENTER;
            break;
        case ResourceBundle::BITMAP_WINDOW_TOP_RIGHT_CORNER:
            resource_id = IDR_WINDOW_TOP_RIGHT_CORNER;
            break;
        case ResourceBundle::BITMAP_WINDOW_LEFT_SIDE:
            resource_id = IDR_WINDOW_LEFT_SIDE;
            break;
        case ResourceBundle::BITMAP_WINDOW_RIGHT_SIDE:
            resource_id = IDR_WINDOW_RIGHT_SIDE;
            break;
        case ResourceBundle::BITMAP_WINDOW_BOTTOM_LEFT_CORNER:
            resource_id = IDR_WINDOW_BOTTOM_LEFT_CORNER;
            break;
        case ResourceBundle::BITMAP_WINDOW_BOTTOM_CENTER:
            resource_id = IDR_WINDOW_BOTTOM_CENTER;
            break;
        case ResourceBundle::BITMAP_WINDOW_BOTTOM_RIGHT_CORNER:
            resource_id = IDR_WINDOW_BOTTOM_RIGHT_CORNER;
            break;
        case ResourceBundle::BITMAP_APP_TOP_LEFT:
            resource_id = IDR_APP_TOP_LEFT;
            break;
        case ResourceBundle::BITMAP_APP_TOP_CENTER:
            resource_id = IDR_APP_TOP_CENTER;
            break;
        case ResourceBundle::BITMAP_APP_TOP_RIGHT:
            resource_id = IDR_APP_TOP_RIGHT;
            break;
        case ResourceBundle::BITMAP_CONTENT_BOTTOM_LEFT_CORNER:
            resource_id = IDR_CONTENT_BOTTOM_LEFT_CORNER;
            break;
        case ResourceBundle::BITMAP_CONTENT_BOTTOM_CENTER:
            resource_id = IDR_CONTENT_BOTTOM_CENTER;
            break;
        case ResourceBundle::BITMAP_CONTENT_BOTTOM_RIGHT_CORNER:
            resource_id = IDR_CONTENT_BOTTOM_RIGHT_CORNER;
            break;
        case ResourceBundle::BITMAP_CONTENT_LEFT_SIDE:
            resource_id = IDR_CONTENT_LEFT_SIDE;
            break;
        case ResourceBundle::BITMAP_CONTENT_RIGHT_SIDE:
            resource_id = IDR_CONTENT_RIGHT_SIDE;
            break;
        case ResourceBundle::BITMAP_CLOSE:
            resource_id = IDR_CLOSE;
            break;
        case ResourceBundle::BITMAP_CLOSE_SA:
            resource_id = IDR_CLOSE_SA;
            break;
        case ResourceBundle::BITMAP_CLOSE_H:
            resource_id = IDR_CLOSE_H;
            break;
        case ResourceBundle::BITMAP_CLOSE_SA_H:
            resource_id = IDR_CLOSE_SA_H;
            break;
        case ResourceBundle::BITMAP_CLOSE_P:
            resource_id = IDR_CLOSE_P;
            break;
        case ResourceBundle::BITMAP_CLOSE_SA_P:
            resource_id = IDR_CLOSE_SA_P;
            break;
        case ResourceBundle::BITMAP_RESTORE:
            resource_id = IDR_RESTORE;
            break;
        case ResourceBundle::BITMAP_RESTORE_H:
            resource_id = IDR_RESTORE_H;
            break;
        case ResourceBundle::BITMAP_RESTORE_P:
            resource_id = IDR_RESTORE_P;
            break;
        case ResourceBundle::BITMAP_MAXIMIZE:
            resource_id = IDR_MAXIMIZE;
            break;
        case ResourceBundle::BITMAP_MAXIMIZE_H:
            resource_id = IDR_MAXIMIZE_H;
            break;
        case ResourceBundle::BITMAP_MAXIMIZE_P:
            resource_id = IDR_MAXIMIZE_P;
            break;
        case ResourceBundle::BITMAP_MINIMIZE:
            resource_id = IDR_MINIMIZE;
            break;
        case ResourceBundle::BITMAP_MINIMIZE_H:
            resource_id = IDR_MINIMIZE_H;
            break;
        case ResourceBundle::BITMAP_MINIMIZE_P:
            resource_id = IDR_MINIMIZE_P;
            break;

        case ResourceBundle::BITMAP_TEXTBUTTON_TOP_LEFT_H:
            resource_id = IDR_TEXTBUTTON_TOP_LEFT_H;
            break;
        case ResourceBundle::BITMAP_TEXTBUTTON_TOP_H:
            resource_id = IDR_TEXTBUTTON_TOP_H;
            break;
        case ResourceBundle::BITMAP_TEXTBUTTON_TOP_RIGHT_H:
            resource_id = IDR_TEXTBUTTON_TOP_RIGHT_H;
            break;
        case ResourceBundle::BITMAP_TEXTBUTTON_LEFT_H:
            resource_id = IDR_TEXTBUTTON_LEFT_H;
            break;
        case ResourceBundle::BITMAP_TEXTBUTTON_CENTER_H:
            resource_id = IDR_TEXTBUTTON_CENTER_H;
            break;
        case ResourceBundle::BITMAP_TEXTBUTTON_RIGHT_H:
            resource_id = IDR_TEXTBUTTON_RIGHT_H;
            break;
        case ResourceBundle::BITMAP_TEXTBUTTON_BOTTOM_LEFT_H:
            resource_id = IDR_TEXTBUTTON_BOTTOM_LEFT_H;
            break;
        case ResourceBundle::BITMAP_TEXTBUTTON_BOTTOM_H:
            resource_id = IDR_TEXTBUTTON_BOTTOM_H;
            break;
        case ResourceBundle::BITMAP_TEXTBUTTON_BOTTOM_RIGHT_H:
            resource_id = IDR_TEXTBUTTON_BOTTOM_RIGHT_H;
            break;
        case ResourceBundle::BITMAP_TEXTBUTTON_TOP_LEFT_P:
            resource_id = IDR_TEXTBUTTON_TOP_LEFT_P;
            break;
        case ResourceBundle::BITMAP_TEXTBUTTON_TOP_P:
            resource_id = IDR_TEXTBUTTON_TOP_P;
            break;
        case ResourceBundle::BITMAP_TEXTBUTTON_TOP_RIGHT_P:
            resource_id = IDR_TEXTBUTTON_TOP_RIGHT_P;
            break;
        case ResourceBundle::BITMAP_TEXTBUTTON_LEFT_P:
            resource_id = IDR_TEXTBUTTON_LEFT_P;
            break;
        case ResourceBundle::BITMAP_TEXTBUTTON_CENTER_P:
            resource_id = IDR_TEXTBUTTON_CENTER_P;
            break;
        case ResourceBundle::BITMAP_TEXTBUTTON_RIGHT_P:
            resource_id = IDR_TEXTBUTTON_RIGHT_P;
            break;
        case ResourceBundle::BITMAP_TEXTBUTTON_BOTTOM_LEFT_P:
            resource_id = IDR_TEXTBUTTON_BOTTOM_LEFT_P;
            break;
        case ResourceBundle::BITMAP_TEXTBUTTON_BOTTOM_P:
            resource_id = IDR_TEXTBUTTON_BOTTOM_P;
            break;
        case ResourceBundle::BITMAP_TEXTBUTTON_BOTTOM_RIGHT_P:
            resource_id = IDR_TEXTBUTTON_BOTTOM_RIGHT_P;
            break;
        }

        return resource_id;
    }
};

int APIENTRY _tWinMain(HINSTANCE hInstance,
                       HINSTANCE hPrevInstance,
                       LPTSTR    lpCmdLine,
                       int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    HRESULT hRes = OleInitialize(NULL);

    // this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
    ::DefWindowProc(NULL, 0, 0, 0L);

    base::AtExitManager exit_manager;

    gfx::GdiplusInitializer gdiplus_initializer;
    gdiplus_initializer.Init();
    ResourceBundle::InitSharedInstance(base::FilePath());
    ResourceBundle::GetSharedInstance().SetIdConveter(
        new AppIdConveter());
    view::AcceleratorHandler handler;
    MessageLoop loop(MessageLoop::TYPE_UI);

    MainWindowDelegate delegate;
    view::Window::CreateWanWindow(NULL, gfx::Rect(), &delegate);
    delegate.window()->SetWindowBounds(gfx::Rect(500, 200, 500, 500), NULL);
    delegate.window()->Show();

    MessageLoopForUI::current()->Run(&handler);

    ResourceBundle::CleanupSharedInstance();

    gdiplus_initializer.UnInit();

    OleUninitialize();
}
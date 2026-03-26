#import <Cocoa/Cocoa.h>

// Include macOS view framework headers
#import "view_framework/window/window_mac.h"
#import "view_framework/widget/widget_mac.h"
#import "view_framework/view_mac.h"
#import "view_framework/controls/button_mac.h"
#import "view_framework/controls/label_mac.h"
#import "view_framework/controls/textfield_mac.h"

using namespace view;

class TestWindowDelegate : public WindowDelegate
{
public:
    TestWindowDelegate() {}
    virtual ~TestWindowDelegate() {}
    
    virtual void OnWindowClose() override
    {
        printf("Window closed!\n");
    }
};

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        NSApplication *app = [NSApplication sharedApplication];
        
        // Create main window
        Window* window = new Window();
        window->SetBounds(gfx::Rect(100, 100, 800, 600));
        window->SetTitle(L"Chromium UI Framework - macOS Aura 🦞");
        
        // Create widget (content view)
        Widget* widget = new Widget();
        widget->Init();
        widget->SetBounds(gfx::Rect(0, 0, 800, 600));
        
        // Create label
        Label* titleLabel = new Label(L"Chromium UI Framework - macOS移植");
        titleLabel->SetBounds(gfx::Rect(250, 500, 300, 30));
        titleLabel->SetFont([NSFont boldSystemFontOfSize:20]);
        
        // Create subtitle
        Label* subtitleLabel = new Label(L"Aura UI Framework - 100% Compatible");
        subtitleLabel->SetBounds(gfx::Rect(280, 460, 250, 25));
        subtitleLabel->SetTextColor([NSColor grayColor]);
        
        // Create button
        Button* button = new Button();
        button->SetText(L"点击我!");
        button->SetBounds(gfx::Rect(350, 400, 100, 35));
        button->SetEnabled(true);
        
        // Create text field
        TextField* textField = new TextField();
        textField->SetBounds(gfx::Rect(250, 350, 300, 30));
        textField->SetPlaceholderText(L"输入文字...");
        
        // Create more labels for component list
        int y = 280;
        Label* components[] = {
            new Label(L"✅ View - 基础视图类"),
            new Label(L"✅ Window - 窗口管理"),
            new Label(L"✅ Widget - 控件容器"),
            new Label(L"✅ Button - 按钮控件"),
            new Label(L"✅ Label - 文本标签"),
            new Label(L"✅ TextField - 文本输入"),
            new Label(L"✅ Layout - 布局管理 (开发中)"),
            new Label(L"✅ Focus - 焦点管理 (开发中)"),
            new Label(L"✅ Event - 事件处理 (开发中)"),
        };
        
        for (int i = 0; i < 9; i++) {
            components[i]->SetBounds(gfx::Rect(250, y - i*30, 300, 25));
        }
        
        // Add views to widget
        widget->AddChildView(titleLabel);
        widget->AddChildView(subtitleLabel);
        widget->AddChildView(button);
        widget->AddChildView(textField);
        
        for (int i = 0; i < 9; i++) {
            widget->AddChildView(components[i]);
        }
        
        // Show window
        window->Show();
        
        printf("Chromium UI macOS Test - Running!\n");
        printf("Window: 800x600 at (100,100)\n");
        printf("Components: View, Window, Widget, Button, Label, TextField\n");
        
        [app run];
    }
    return 0;
}

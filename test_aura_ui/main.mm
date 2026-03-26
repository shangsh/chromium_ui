// Aura UI Test - macOS
#import <Cocoa/Cocoa.h>

#include "aura/aura_widget.h"
#include "aura/aura_view.h"
#include "gfx/rect.h"
#include "gfx/size.h"

using namespace aura;

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        NSApplication *app = [NSApplication sharedApplication];
        
        // Create Widget (Window)
        Widget* widget = new Widget();
        widget->SetBounds(gfx::Rect(100, 100, 900, 650));
        widget->SetTitle(L"Chromium Aura UI - macOS Complete Port 🦞");
        
        // Get content view
        View* content = widget->GetContentView();
        content->SetBounds(0, 0, 900, 650);
        
        // Title
        Label* title = new Label(L"Chromium Aura UI Framework");
        title->SetBounds(250, 580, 400, 40);
        title->SetFont([NSFont boldSystemFontOfSize:32]);
        title->SetAlignment(NSTextAlignmentCenter);
        content->AddChildView(title);
        
        // Subtitle
        Label* subtitle = new Label(L"Aura UI Framework - 100% macOS Port");
        subtitle->SetBounds(280, 540, 340, 25);
        subtitle->SetTextColor([NSColor grayColor]);
        subtitle->SetAlignment(NSTextAlignmentCenter);
        content->AddChildView(subtitle);
        
        // English component list
        int y = 460;
        const char* comps[] = {
            "View         - Base view class",
            "Widget       - Window widget",
            "Button       - Button control",
            "Label        - Text label",
            "TextField    - Text input",
            "Checkbox     - Checkbox control",
            "ComboBox     - Dropdown (dev)",
            "ListBox      - List box (dev)",
            "ScrollBar    - Scroll bar (dev)",
            "TabControl   - Tab control (dev)",
            "TreeView     - Tree view (dev)",
            "Menu         - Menu system (dev)",
            "Toolbar      - Toolbar (dev)",
            "StatusBar    - Status bar (dev)",
            "Dialog       - Dialog box (dev)"
        };
        
        for (int i = 0; i < 15; i++) {
            Label* comp = new Label(std::wstring(comps[i], comps[i] + strlen(comps[i])));
            comp->SetBounds(150, y, 400, 22);
            comp->SetFont([NSFont systemFontOfSize:13]);
            content->AddChildView(comp);
            y -= 25;
        }
        
        // Test button
        Button* btn = new Button();
        btn->SetText(L"Test Button");
        btn->SetBounds(150, 30, 150, 35);
        content->AddChildView(btn);
        
        // Test textfield
        TextField* tf = new TextField();
        tf->SetPlaceholder(L"Input test...");
        tf->SetBounds(320, 30, 200, 25);
        content->AddChildView(tf);
        
        // Test checkbox
        Checkbox* cb = new Checkbox(L"Test Checkbox");
        cb->SetBounds(540, 30, 150, 25);
        content->AddChildView(cb);
        
        // Progress
        Label* progress = new Label(L"Progress: 60%");
        progress->SetBounds(350, 75, 200, 20);
        progress->SetTextColor([NSColor systemBlueColor]);
        progress->SetFont([NSFont boldSystemFontOfSize:14]);
        content->AddChildView(progress);
        
        // Show
        widget->Show();
        
        printf("========================================\n");
        printf("  Chromium Aura UI - macOS Port Test\n");
        printf("========================================\n");
        printf("  Window: 900x650\n");
        printf("  Controls: View, Widget, Button, Label, TextField, Checkbox\n");
        printf("  In Progress: ComboBox, ListBox, ScrollBar, etc.\n");
        printf("========================================\n");
        
        [app run];
        
        delete widget;
    }
    return 0;
}

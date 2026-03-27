// demo_mac - Aura UI macOS Demo
// Uses aura_mac framework controls: ButtonMac, LabelMac, TextFieldMac, CheckboxMac

#import <Cocoa/Cocoa.h>
#include <string>
#include <iostream>
#include "aura_mac/controls/controls_mac.h"
#include "aura_mac/view/view_mac.h"

using namespace aura::mac;

@interface DemoAppDelegate : NSObject <NSApplicationDelegate>
@end

@implementation DemoAppDelegate {
    NSWindow* window_;
    ButtonMac* helloBtn_;
    LabelMac* statusLabel_;
    TextFieldMac* nameField_;
    int clickCount_;
}

- (void)applicationDidFinishLaunching:(NSNotification*)notification {
    clickCount_ = 0;
    
    // Create main window
    NSRect frame = NSMakeRect(0, 0, 800, 600);
    window_ = [[NSWindow alloc] initWithContentRect:frame
                                         styleMask:NSWindowStyleMaskTitled |
                                                  NSWindowStyleMaskClosable |
                                                  NSWindowStyleMaskMiniaturizable |
                                                  NSWindowStyleMaskResizable
                                           backing:NSBackingStoreBuffered
                                             defer:NO];
    [window_ setTitle:@"Aura UI Framework - macOS Demo 🦞"];
    [window_ center];

    NSView* contentView = [window_ contentView];
    [contentView setWantsLayer:YES];
    [[contentView layer] setBackgroundColor:[NSColor windowBackgroundColor].CGColor];

    // ---- Title ----
    LabelMac* title = new LabelMac();
    title->Init(50, 510, 700, 40, L"Aura UI Framework - macOS Demo");
    title->SetFont([NSFont systemFontOfSize:24 weight:NSFontWeightBold]);
    title->SetTextColor([NSColor labelColor]);
    title->SetAlignment(NSTextAlignmentCenter);
    [contentView addSubview:title->GetNSView()];

    // ---- Subtitle ----
    LabelMac* subtitle = new LabelMac();
    subtitle->Init(50, 470, 700, 30, L"Cross-platform UI framework running natively on macOS");
    subtitle->SetFont([NSFont systemFontOfSize:14 weight:NSFontWeightRegular]);
    subtitle->SetTextColor([NSColor secondaryLabelColor]);
    subtitle->SetAlignment(NSTextAlignmentCenter);
    [contentView addSubview:subtitle->GetNSView()];

    // ---- Separator ----
    NSBox* separator = [[NSBox alloc] initWithFrame:NSMakeRect(50, 455, 700, 1)];
    [separator setBoxType:NSBoxSeparator];
    [contentView addSubview:separator];

    // ---- Hello Button ----
    helloBtn_ = new ButtonMac();
    helloBtn_->Init(100, 370, 200, 44);
    helloBtn_->SetText(L"Click Me! 👋");
    helloBtn_->SetOnClick([self]() {
        clickCount_++;
        NSString* msg;
        if (clickCount_ == 1) {
            msg = @"✅ Button clicked! Aura UI is working! 🎉";
        } else {
            msg = [NSString stringWithFormat:@"✅ Clicked %d times! Aura UI works great! 🎉", clickCount_];
        }
        NSTextField* statusField = (NSTextField*)statusLabel_->GetNSView();
        [statusField setStringValue:msg];
        [statusField setTextColor:[NSColor systemGreenColor]];
        [statusField setFont:[NSFont systemFontOfSize:14 weight:NSFontWeightMedium]];
        
        NSString* btnTitle = [NSString stringWithFormat:@"Clicked! (%d)", clickCount_];
        std::string s8 = [btnTitle UTF8String];
        std::wstring s(s8.begin(), s8.end());
        helloBtn_->SetText(s);
    });
    [contentView addSubview:helloBtn_->GetNSView()];

    // ---- Disabled Button ----
    ButtonMac* disabledBtn = new ButtonMac();
    disabledBtn->Init(320, 370, 200, 44);
    disabledBtn->SetText(L"Disabled Button");
    disabledBtn->SetEnabled(false);
    [contentView addSubview:disabledBtn->GetNSView()];

    // ---- GitHub Button ----
    ButtonMac* githubBtn = new ButtonMac();
    githubBtn->Init(540, 370, 200, 44);
    githubBtn->SetText(L"View Source →");
    githubBtn->SetOnClick([]() {
        [[NSWorkspace sharedWorkspace] openURL:
            [NSURL URLWithString:@"https://github.com/shangsh/chromium_ui"]];
    });
    [contentView addSubview:githubBtn->GetNSView()];

    // ---- Status Label ----
    statusLabel_ = new LabelMac();
    statusLabel_->Init(100, 310, 640, 35, L"👆 Click the button to see Aura UI respond!");
    statusLabel_->SetFont([NSFont systemFontOfSize:14 weight:NSFontWeightRegular]);
    statusLabel_->SetTextColor([NSColor systemBlueColor]);
    statusLabel_->SetAlignment(NSTextAlignmentCenter);
    [contentView addSubview:statusLabel_->GetNSView()];

    // ---- Framework Info ----
    LabelMac* infoTitle = new LabelMac();
    infoTitle->Init(50, 250, 700, 25, L"Framework Components (all running on macOS):");
    infoTitle->SetFont([NSFont systemFontOfSize:13 weight:NSFontWeightSemibold]);
    infoTitle->SetTextColor([NSColor labelColor]);
    [contentView addSubview:infoTitle->GetNSView()];

    const wchar_t* lines[] = {
        L"✅  aura::WidgetMac     — NSWindow abstraction, Init/Show/Close",
        L"✅  aura::ButtonMac     — NSButton wrapper, SetText/SetOnClick",
        L"✅  aura::LabelMac      — NSTextField wrapper, SetFont/SetTextColor",
        L"✅  aura::TextFieldMac  — NSTextField wrapper, SetPlaceholder/SetOnChange",
        L"✅  aura::CheckboxMac   — NSButton(switch) wrapper, SetChecked",
        L"✅  gfx::Canvas         — CoreGraphics rendering path",
    };
    for (int i = 0; i < 6; i++) {
        LabelMac* line = new LabelMac();
        line->Init(70, 215 - i * 24, 660, 20, lines[i]);
        line->SetFont([NSFont monospacedSystemFontOfSize:12 weight:NSFontWeightRegular]);
        line->SetTextColor([NSColor secondaryLabelColor]);
        [contentView addSubview:line->GetNSView()];
    }

    // ---- TextField Demo ----
    LabelMac* nameLabel = new LabelMac();
    nameLabel->Init(50, 70, 80, 24, L"Name:");
    nameLabel->SetFont([NSFont systemFontOfSize:13 weight:NSFontWeightMedium]);
    nameLabel->SetTextColor([NSColor labelColor]);
    [contentView addSubview:nameLabel->GetNSView()];

    nameField_ = new TextFieldMac();
    nameField_->Init(135, 68, 320, 26);
    nameField_->SetPlaceholder(L"Type something...");
    nameField_->SetOnChange([self]() {
        std::wstring t = nameField_->GetText();
        std::wcout << L"[Aura UI] TextField changed: " << t << std::endl;
    });
    [contentView addSubview:nameField_->GetNSView()];

    // ---- Checkbox Demo ----
    CheckboxMac* agreeCheck = new CheckboxMac();
    agreeCheck->Init(470, 64, 280, 26);
    agreeCheck->SetText(L"I agree to use Aura UI");
    [contentView addSubview:agreeCheck->GetNSView()];

    // ---- Footer ----
    LabelMac* footer = new LabelMac();
    footer->Init(50, 25, 700, 20,
        L"chromium_ui macOS Port  |  github.com/shangsh/chromium_ui");
    footer->SetFont([NSFont systemFontOfSize:11 weight:NSFontWeightRegular]);
    footer->SetTextColor([NSColor tertiaryLabelColor]);
    footer->SetAlignment(NSTextAlignmentCenter);
    [contentView addSubview:footer->GetNSView()];

    [window_ makeKeyAndOrderFront:nil];
    [NSApp activateIgnoringOtherApps:YES];
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication*)sender {
    return YES;
}

@end

int main(int argc, char* argv[]) {
    @autoreleasepool {
        [NSApplication sharedApplication];
        DemoAppDelegate* delegate = [[DemoAppDelegate alloc] init];
        [NSApp setDelegate:delegate];
        [NSApp run];
    }
    return 0;
}

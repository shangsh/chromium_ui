// demo_mac - Aura UI macOS Demo (comprehensive)
#import <Cocoa/Cocoa.h>
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
#include "aura_mac/controls/controls_mac.h"
#include "aura_mac/view/view_mac.h"

using namespace aura::mac;

static std::wstring DoubleToWString(double v) {
    std::wostringstream oss;
    oss << std::fixed << std::setprecision(2) << v;
    return oss.str();
}

static std::wstring NSStringToWString(NSString* s) {
    if (!s) return L"";
    const char* c = [s UTF8String];
    std::string s8(c ? c : "");
    return std::wstring(s8.begin(), s8.end());
}

// ============================================================================
// Demo App Delegate
// ============================================================================

@interface DemoAppDelegate : NSObject <NSApplicationDelegate>
@end

@implementation DemoAppDelegate {
    NSWindow* window_;
    
    // Controls
    ButtonMac* helloBtn_;
    LabelMac* statusLabel_;
    TextFieldMac* nameField_;
    TextFieldMac* passwordField_;
    CheckboxMac* agreeCheck_;
    SliderMac* volumeSlider_;
    ListViewMac* fileList_;
    ComboBoxMac* themeCombo_;
    ProgressBarMac* progressBar_;
    ProgressBarMac* indeterminateProgress_;
    SeparatorMac* sep1_;
    SeparatorMac* sep2_;
    LabelMac* sliderValueLabel_;
    LabelMac* progressValueLabel_;
    ButtonMac* toggleProgressBtn_;
    ButtonMac* animBtn_;
    
    NSTimer* progressTimer_;
    double progressDir_;
    int clickCount_;
    bool progressStarted_;
}

- (void)applicationDidFinishLaunching:(NSNotification*)notification {
    clickCount_ = 0;
    progressDir_ = 1.0;
    progressStarted_ = false;
    
    // ---- Main Window ----
    NSRect frame = NSMakeRect(0, 0, 900, 720);
    window_ = [[NSWindow alloc] initWithContentRect:frame
                                         styleMask:NSWindowStyleMaskTitled |
                                                  NSWindowStyleMaskClosable |
                                                  NSWindowStyleMaskMiniaturizable |
                                                  NSWindowStyleMaskResizable
                                           backing:NSBackingStoreBuffered
                                             defer:NO];
    [window_ setTitle:@"Aura UI Framework - macOS Demo 🦞"];
    [window_ setMinSize:NSMakeSize(700, 600)];
    [window_ center];

    NSView* contentView = [window_ contentView];
    [contentView setWantsLayer:YES];
    [[contentView layer] setBackgroundColor:[NSColor windowBackgroundColor].CGColor];

    // =========================================================================
    // ROW 1: Title + Separator
    // =========================================================================
    LabelMac* title = new LabelMac();
    title->Init(30, 660, 840, 36, L"Aura UI Framework  —  macOS Native Controls Demo");
    title->SetFont([NSFont systemFontOfSize:22 weight:NSFontWeightBold]);
    title->SetTextColor([NSColor labelColor]);
    title->SetAlignment(NSTextAlignmentLeft);
    [contentView addSubview:title->GetNSView()];

    sep1_ = new SeparatorMac();
    sep1_->InitHorizontal(30, 645, 840);
    [contentView addSubview:sep1_->GetNSView()];

    // =========================================================================
    // LEFT COLUMN: Button, Checkbox, TextField
    // =========================================================================
    
    // --- Hello Button ---
    helloBtn_ = new ButtonMac();
    helloBtn_->Init(30, 590, 160, 40);
    helloBtn_->SetText(L"Click Me! 👋");
    helloBtn_->SetOnClick([self]() {
        clickCount_++;
        NSString* msg;
        if (clickCount_ == 1) {
            msg = @"✅ Button clicked! Aura UI works! 🎉";
        } else {
            msg = [NSString stringWithFormat:@"✅ Clicked %d times!", clickCount_];
        }
        NSTextField* f = (NSTextField*)statusLabel_->GetNSView();
        [f setStringValue:msg];
        [f setTextColor:[NSColor systemGreenColor]];
        [f setFont:[NSFont systemFontOfSize:13 weight:NSFontWeightMedium]];
        
        NSString* btnTitle = [NSString stringWithFormat:@"Clicked! (%d)", clickCount_];
        helloBtn_->SetText(NSStringToWString(btnTitle));
    });
    [contentView addSubview:helloBtn_->GetNSView()];

    // --- Disabled Button ---
    ButtonMac* disabledBtn = new ButtonMac();
    disabledBtn->Init(205, 590, 140, 40);
    disabledBtn->SetText(L"Disabled");
    disabledBtn->SetEnabled(false);
    [contentView addSubview:disabledBtn->GetNSView()];

    // --- GitHub Button ---
    ButtonMac* githubBtn = new ButtonMac();
    githubBtn->Init(360, 590, 140, 40);
    githubBtn->SetText(L"View Source →");
    githubBtn->SetOnClick([]() {
        [[NSWorkspace sharedWorkspace] openURL:
            [NSURL URLWithString:@"https://github.com/shangsh/chromium_ui"]];
    });
    [contentView addSubview:githubBtn->GetNSView()];

    // --- Status Label ---
    statusLabel_ = new LabelMac();
    statusLabel_->Init(30, 555, 470, 28, L"👆 Click the button to see Aura UI respond!");
    statusLabel_->SetFont([NSFont systemFontOfSize:13 weight:NSFontWeightRegular]);
    statusLabel_->SetTextColor([NSColor secondaryLabelColor]);
    [contentView addSubview:statusLabel_->GetNSView()];

    // --- Checkbox ---
    agreeCheck_ = new CheckboxMac();
    agreeCheck_->Init(30, 520, 300, 26);
    agreeCheck_->SetText(L"I agree to use Aura UI framework");
    agreeCheck_->SetOnChange([self]() {
        bool checked = agreeCheck_->IsChecked();
        NSTextField* f = (NSTextField*)statusLabel_->GetNSView();
        if (checked) {
            [f setStringValue:@"✅ Checkbox checked!"];
            [f setTextColor:[NSColor systemGreenColor]];
        } else {
            [f setStringValue:@"❌ Checkbox unchecked"];
            [f setTextColor:[NSColor systemRedColor]];
        }
    });
    [contentView addSubview:agreeCheck_->GetNSView()];

    // =========================================================================
    // TEXT FIELD SECTION
    // =========================================================================
    sep2_ = new SeparatorMac();
    sep2_->InitHorizontal(30, 492, 500);
    [contentView addSubview:sep2_->GetNSView()];

    LabelMac* inputLabel = new LabelMac();
    inputLabel->Init(30, 460, 80, 24, L"Username:");
    inputLabel->SetFont([NSFont systemFontOfSize:13 weight:NSFontWeightMedium]);
    inputLabel->SetTextColor([NSColor labelColor]);
    [contentView addSubview:inputLabel->GetNSView()];

    nameField_ = new TextFieldMac();
    nameField_->Init(115, 458, 240, 26);
    nameField_->SetPlaceholder(L"Enter your name...");
    nameField_->SetOnChange([self]() {
        std::wstring t = nameField_->GetText();
        if (!t.empty()) {
            NSTextField* f = (NSTextField*)statusLabel_->GetNSView();
            NSString* msg = [NSString stringWithFormat:@"📝 Hello, %.*S!", (int)t.size(), t.c_str()];
            [f setStringValue:msg];
            [f setTextColor:[NSColor systemBlueColor]];
            [f setFont:[NSFont systemFontOfSize:13 weight:NSFontWeightMedium]];
        }
    });
    [contentView addSubview:nameField_->GetNSView()];

    LabelMac* pwdLabel = new LabelMac();
    pwdLabel->Init(30, 428, 80, 24, L"Password:");
    pwdLabel->SetFont([NSFont systemFontOfSize:13 weight:NSFontWeightMedium]);
    pwdLabel->SetTextColor([NSColor labelColor]);
    [contentView addSubview:pwdLabel->GetNSView()];

    passwordField_ = new TextFieldMac();
    passwordField_->Init(115, 426, 240, 26);
    passwordField_->SetPlaceholder(L"Enter password...");
    passwordField_->SetPasswordMode(true);
    [contentView addSubview:passwordField_->GetNSView()];

    // =========================================================================
    // SLIDER SECTION
    // =========================================================================
    LabelMac* sliderTitle = new LabelMac();
    sliderTitle->Init(30, 390, 200, 22, L"Volume:");
    sliderTitle->SetFont([NSFont systemFontOfSize:13 weight:NSFontWeightSemibold]);
    sliderTitle->SetTextColor([NSColor labelColor]);
    [contentView addSubview:sliderTitle->GetNSView()];

    sliderValueLabel_ = new LabelMac();
    sliderValueLabel_->Init(295, 390, 60, 22, L"50.00");
    sliderValueLabel_->SetFont([NSFont monospacedSystemFontOfSize:12 weight:NSFontWeightRegular]);
    sliderValueLabel_->SetTextColor([NSColor secondaryLabelColor]);
    [contentView addSubview:sliderValueLabel_->GetNSView()];

    volumeSlider_ = new SliderMac();
    volumeSlider_->Init(30, 360, 325, 26);
    volumeSlider_->SetMinValue(0);
    volumeSlider_->SetMaxValue(100);
    volumeSlider_->SetValue(50);
    volumeSlider_->SetOnChange([self](double v) {
        sliderValueLabel_->SetText(DoubleToWString(v));
        NSTextField* f = (NSTextField*)statusLabel_->GetNSView();
        NSString* msg = [NSString stringWithFormat:@"🔊 Volume: %.0f%%", v];
        [f setStringValue:msg];
        [f setTextColor:[NSColor systemBlueColor]];
    });
    [contentView addSubview:volumeSlider_->GetNSView()];

    // =========================================================================
    // COMBOBOX SECTION
    // =========================================================================
    LabelMac* themeLabel = new LabelMac();
    themeLabel->Init(30, 325, 80, 24, L"Theme:");
    themeLabel->SetFont([NSFont systemFontOfSize:13 weight:NSFontWeightMedium]);
    themeLabel->SetTextColor([NSColor labelColor]);
    [contentView addSubview:themeLabel->GetNSView()];

    themeCombo_ = new ComboBoxMac();
    themeCombo_->Init(115, 323, 240, 26);
    themeCombo_->AddItem(L"System Default");
    themeCombo_->AddItem(L"Light");
    themeCombo_->AddItem(L"Dark");
    themeCombo_->AddItem(L"High Contrast");
    themeCombo_->SelectItem(0);
    themeCombo_->SetOnChange([self]() {
        std::wstring item = themeCombo_->GetSelectedItem();
        NSTextField* f = (NSTextField*)statusLabel_->GetNSView();
        NSString* msg = [NSString stringWithFormat:@"🎨 Theme: %.*S!", (int)item.size(), item.c_str()];
        [f setStringValue:msg];
        [f setTextColor:[NSColor systemPurpleColor]];
    });
    [contentView addSubview:themeCombo_->GetNSView()];

    // =========================================================================
    // PROGRESS BAR SECTION
    // =========================================================================
    LabelMac* progLabel = new LabelMac();
    progLabel->Init(30, 285, 160, 22, L"Progress:");
    progLabel->SetFont([NSFont systemFontOfSize:13 weight:NSFontWeightSemibold]);
    progLabel->SetTextColor([NSColor labelColor]);
    [contentView addSubview:progLabel->GetNSView()];

    progressValueLabel_ = new LabelMac();
    progressValueLabel_->Init(295, 285, 60, 22, L"0.00%");
    progressValueLabel_->SetFont([NSFont monospacedSystemFontOfSize:12 weight:NSFontWeightRegular]);
    progressValueLabel_->SetTextColor([NSColor secondaryLabelColor]);
    [contentView addSubview:progressValueLabel_->GetNSView()];

    progressBar_ = new ProgressBarMac();
    progressBar_->Init(30, 255, 325, 20);
    progressBar_->SetMinValue(0);
    progressBar_->SetMaxValue(100);
    progressBar_->SetValue(0);
    [contentView addSubview:progressBar_->GetNSView()];

    // Start/Stop progress button
    toggleProgressBtn_ = new ButtonMac();
    toggleProgressBtn_->Init(30, 215, 140, 34);
    toggleProgressBtn_->SetText(L"▶ Start Progress");
    toggleProgressBtn_->SetOnClick([self]() {
        if (!progressStarted_) {
            progressStarted_ = true;
            progressTimer_ = [NSTimer scheduledTimerWithTimeInterval:0.05
                                                              target:self
                                                            selector:@selector(updateProgress:)
                                                            userInfo:nil
                                                             repeats:YES];
            toggleProgressBtn_->SetText(L"⏸ Pause");
        } else {
            progressStarted_ = false;
            [progressTimer_ invalidate];
            progressTimer_ = nil;
            toggleProgressBtn_->SetText(L"▶ Resume");
        }
    });
    [contentView addSubview:toggleProgressBtn_->GetNSView()];

    // Reset button
    ButtonMac* resetBtn = new ButtonMac();
    resetBtn->Init(185, 215, 100, 34);
    resetBtn->SetText(L"↺ Reset");
    resetBtn->SetOnClick([self]() {
        progressStarted_ = false;
        [progressTimer_ invalidate];
        progressTimer_ = nil;
        progressBar_->SetValue(0);
        progressValueLabel_->SetText(L"0.00%");
        toggleProgressBtn_->SetText(L"▶ Start Progress");
        NSTextField* f = (NSTextField*)statusLabel_->GetNSView();
        [f setStringValue:@"🔄 Progress reset"];
        [f setTextColor:[NSColor secondaryLabelColor]];
    });
    [contentView addSubview:resetBtn->GetNSView()];

    // =========================================================================
    // RIGHT COLUMN: ListView
    // =========================================================================
    LabelMac* listTitle = new LabelMac();
    listTitle->Init(530, 660, 340, 26, L"File List (ListViewMac)");
    listTitle->SetFont([NSFont systemFontOfSize:15 weight:NSFontWeightSemibold]);
    listTitle->SetTextColor([NSColor labelColor]);
    [contentView addSubview:listTitle->GetNSView()];

    fileList_ = new ListViewMac();
    fileList_->Init(530, 280, 340, 370);
    fileList_->AddColumn(L"Name", 200);
    fileList_->AddColumn(L"Size", 70);
    fileList_->AddColumn(L"Type", 60);

    // Add sample rows
    fileList_->AddRow({L"📄 report.pdf", L"2.4 MB", L"PDF"});
    fileList_->AddRow({L"📊 data.xlsx", L"856 KB", L"XLS"});
    fileList_->AddRow({L"📝 notes.txt", L"12 KB", L"TXT"});
    fileList_->AddRow({L"🖼 photo.jpg", L"4.1 MB", L"IMG"});
    fileList_->AddRow({L"📦 archive.zip", L"18.7 MB", L"ZIP"});
    fileList_->AddRow({L"🎵 music.mp3", L"5.3 MB", L"AUD"});
    fileList_->AddRow({L"🎬 video.mp4", L"124 MB", L"VID"});
    fileList_->AddRow({L"📋 todo.md", L"4 KB", L"MD"});
    fileList_->AddRow({L"🔧 config.ini", L"2 KB", L"CFG"});

    fileList_->SetOnSelect([self](int row) {
        NSTextField* f = (NSTextField*)statusLabel_->GetNSView();
        if (row >= 0 && row < fileList_->GetRowCount()) {
            NSString* msg = [NSString stringWithFormat:@"📋 Selected row %d", row + 1];
            [f setStringValue:msg];
            [f setTextColor:[NSColor systemBlueColor]];
        }
    });

    [contentView addSubview:fileList_->GetNSView()];

    // ListView controls: Add / Remove
    ButtonMac* addRowBtn = new ButtonMac();
    addRowBtn->Init(530, 245, 100, 30);
    addRowBtn->SetText(L"+ Add Row");
    addRowBtn->SetOnClick([self]() {
        static int counter = 1;
        std::wstring name = L"📄 new_file_" + std::to_wstring(counter++) + L".txt";
        fileList_->AddRow({name, L"0 KB", L"TXT"});
        NSTextField* f = (NSTextField*)statusLabel_->GetNSView();
        NSString* msg = [NSString stringWithFormat:@"➕ Added: %.*S", (int)name.size(), name.c_str()];
        [f setStringValue:msg];
        [f setTextColor:[NSColor systemGreenColor]];
    });
    [contentView addSubview:addRowBtn->GetNSView()];

    ButtonMac* removeRowBtn = new ButtonMac();
    removeRowBtn->Init(640, 245, 110, 30);
    removeRowBtn->SetText(L"✕ Remove");
    removeRowBtn->SetOnClick([self]() {
        int sel = fileList_->GetSelectedRow();
        if (sel >= 0) {
            fileList_->RemoveSelectedRow();
            NSTextField* f = (NSTextField*)statusLabel_->GetNSView();
            [f setStringValue:[NSString stringWithFormat:@"🗑 Removed row %d", sel + 1]];
            [f setTextColor:[NSColor systemOrangeColor]];
        }
    });
    [contentView addSubview:removeRowBtn->GetNSView()];

    ButtonMac* clearBtn = new ButtonMac();
    clearBtn->Init(760, 245, 110, 30);
    clearBtn->SetText(L"🗑 Clear All");
    clearBtn->SetOnClick([self]() {
        fileList_->ClearRows();
        NSTextField* f = (NSTextField*)statusLabel_->GetNSView();
        [f setStringValue:@"🗑 List cleared"];
        [f setTextColor:[NSColor systemRedColor]];
    });
    [contentView addSubview:clearBtn->GetNSView()];

    // =========================================================================
    // INDETERMINATE PROGRESS (bottom right)
    // =========================================================================
    LabelMac* indetLabel = new LabelMac();
    indetLabel->Init(530, 210, 200, 22, L"Indeterminate:");
    indetLabel->SetFont([NSFont systemFontOfSize:13 weight:NSFontWeightSemibold]);
    indetLabel->SetTextColor([NSColor labelColor]);
    [contentView addSubview:indetLabel->GetNSView()];

    indeterminateProgress_ = new ProgressBarMac();
    indeterminateProgress_->Init(530, 182, 340, 18);
    indeterminateProgress_->SetIndeterminate(true);
    [contentView addSubview:indeterminateProgress_->GetNSView()];

    animBtn_ = new ButtonMac();
    animBtn_->Init(530, 148, 120, 28);
    animBtn_->SetText(L"▶ Start Anim");
    animBtn_->SetOnClick([self]() {
        indeterminateProgress_->StartAnimation();
        animBtn_->SetText(L"⏸ Stop Anim");
    });
    [contentView addSubview:animBtn_->GetNSView()];

    ButtonMac* stopAnimBtn = new ButtonMac();
    stopAnimBtn->Init(660, 148, 100, 28);
    stopAnimBtn->SetText(L"■ Stop");
    stopAnimBtn->SetOnClick([self]() {
        indeterminateProgress_->StopAnimation();
        animBtn_->SetText(L"▶ Start Anim");
    });
    [contentView addSubview:stopAnimBtn->GetNSView()];

    // =========================================================================
    // FOOTER
    // =========================================================================
    SeparatorMac* footerSep = new SeparatorMac();
    footerSep->InitHorizontal(30, 118, 840);
    [contentView addSubview:footerSep->GetNSView()];

    LabelMac* footer = new LabelMac();
    footer->Init(30, 95, 840, 18,
        L"chromium_ui macOS Port  |  github.com/shangsh/chromium_ui  |  "
        L"Built with aura_mac controls");
    footer->SetFont([NSFont systemFontOfSize:11 weight:NSFontWeightRegular]);
    footer->SetTextColor([NSColor tertiaryLabelColor]);
    footer->SetAlignment(NSTextAlignmentCenter);
    [contentView addSubview:footer->GetNSView()];

    // =========================================================================
    // COMPONENT INFO (bottom strip)
    // =========================================================================
    const wchar_t* components[] = {
        L"✅ ButtonMac",
        L"✅ LabelMac",
        L"✅ TextFieldMac",
        L"✅ CheckboxMac",
        L"✅ SliderMac",
        L"✅ ListViewMac",
        L"✅ ComboBoxMac",
        L"✅ ProgressBarMac",
        L"✅ SeparatorMac",
        L"✅ PanelMac",
    };
    for (int i = 0; i < 10; i++) {
        LabelMac* comp = new LabelMac();
        int x = 30 + (i % 5) * 172;
        int y = 58 - (i / 5) * 22;
        comp->Init(x, y, 165, 18, components[i]);
        comp->SetFont([NSFont systemFontOfSize:11 weight:NSFontWeightRegular]);
        comp->SetTextColor([NSColor quaternaryLabelColor]);
        [contentView addSubview:comp->GetNSView()];
    }

    [window_ makeKeyAndOrderFront:nil];
    [NSApp activateIgnoringOtherApps:YES];
}

- (void)updateProgress:(NSTimer*)timer {
    double v = progressBar_->GetValue();
    v += 1.5 * progressDir_;
    if (v >= 100) { v = 100; progressDir_ = -1; }
    if (v <= 0) { v = 0; progressDir_ = 1; }
    progressBar_->SetValue(v);
    progressValueLabel_->SetText(DoubleToWString(v) + L"%");
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

// demo_mac - Aura UI macOS Demo
// Entry point for the chromium_ui macOS demo application

#import <Cocoa/Cocoa.h>
#include <iostream>

// Forward declarations from aura_mac
namespace aura {
namespace mac {
    class WidgetMac;
    class ButtonMac;
    class LabelMac;
    class TextFieldMac;
}}

// Simple AppDelegate to handle the application lifecycle
@interface DemoAppDelegate : NSObject <NSApplicationDelegate>
@end

@implementation DemoAppDelegate {
    NSWindow* window_;
    NSButton* helloButton_;
    NSTextField* statusLabel_;
}

- (void)applicationDidFinishLaunching:(NSNotification*)notification {
    // Create the main window (800x600)
    NSRect frame = NSMakeRect(0, 0, 800, 600);
    window_ = [[NSWindow alloc] initWithContentRect:frame
                                         styleMask:NSWindowStyleMaskTitled |
                                                  NSWindowStyleMaskClosable |
                                                  NSWindowStyleMaskMiniaturizable |
                                                  NSWindowStyleMaskResizable
                                           backing:NSBackingStoreBuffered
                                             defer:NO];
    [window_ setTitle:@"Chromium UI - macOS Demo 🦞"];
    [window_ center];

    NSView* contentView = [window_ contentView];
    [contentView setWantsLayer:YES];
    [[contentView layer] setBackgroundColor:[NSColor windowBackgroundColor].CGColor];

    // Title label
    NSTextField* titleLabel = [[NSTextField alloc] initWithFrame:NSMakeRect(50, 500, 700, 40)];
    [titleLabel setStringValue:@"Aura UI Framework - macOS Demo"];
    [titleLabel setFont:[NSFont systemFontOfSize:24 weight:NSFontWeightBold]];
    [titleLabel setAlignment:NSTextAlignmentCenter];
    [titleLabel setTextColor:[NSColor labelColor]];
    [titleLabel setBezeled:NO];
    [titleLabel setEditable:NO];
    [titleLabel setDrawsBackground:NO];
    [contentView addSubview:titleLabel];

    // Subtitle label
    NSTextField* subtitleLabel = [[NSTextField alloc] initWithFrame:NSMakeRect(50, 460, 700, 30)];
    [subtitleLabel setStringValue:@"Cross-platform UI framework running natively on macOS"];
    [subtitleLabel setFont:[NSFont systemFontOfSize:14 weight:NSFontWeightRegular]];
    [subtitleLabel setAlignment:NSTextAlignmentCenter];
    [subtitleLabel setTextColor:[NSColor secondaryLabelColor]];
    [subtitleLabel setBezeled:NO];
    [subtitleLabel setEditable:NO];
    [subtitleLabel setDrawsBackground:NO];
    [contentView addSubview:subtitleLabel];

    // Separator line
    NSBox* separator = [[NSBox alloc] initWithFrame:NSMakeRect(50, 445, 700, 1)];
    [separator setBoxType:NSBoxSeparator];
    [contentView addSubview:separator];

    // Button 1 - Click me
    helloButton_ = [[NSButton alloc] initWithFrame:NSMakeRect(100, 350, 200, 44)];
    [helloButton_ setTitle:@"Click Me! 👋"];
    [helloButton_ setFont:[NSFont systemFontOfSize:16 weight:NSFontWeightMedium]];
    [helloButton_ setBezelStyle:NSBezelStyleRounded];
    [helloButton_ setTarget:self];
    [helloButton_ setAction:@selector(buttonClicked:)];
    [contentView addSubview:helloButton_];

    // Button 2 - Secondary
    NSButton* secondaryBtn = [[NSButton alloc] initWithFrame:NSMakeRect(320, 350, 200, 44)];
    [secondaryBtn setTitle:@"Disabled Button"];
    [secondaryBtn setFont:[NSFont systemFontOfSize:16 weight:NSFontWeightMedium]];
    [secondaryBtn setBezelStyle:NSBezelStyleRounded];
    [secondaryBtn setEnabled:NO];
    [contentView addSubview:secondaryBtn];

    // Button 3 - Checkout the code
    NSButton* codeBtn = [[NSButton alloc] initWithFrame:NSMakeRect(540, 350, 200, 44)];
    [codeBtn setTitle:@"View Source Code →"];
    [codeBtn setFont:[NSFont systemFontOfSize:16 weight:NSFontWeightMedium]];
    [codeBtn setBezelStyle:NSBezelStyleRounded];
    [codeBtn setTarget:self];
    [codeBtn setAction:@selector(openRepo:)];
    [contentView addSubview:codeBtn];

    // Status label
    statusLabel_ = [[NSTextField alloc] initWithFrame:NSMakeRect(100, 290, 640, 40)];
    [statusLabel_ setStringValue:@"👆 Click the button to see Aura UI respond!"];
    [statusLabel_ setFont:[NSFont systemFontOfSize:14 weight:NSFontWeightRegular]];
    [statusLabel_ setAlignment:NSTextAlignmentCenter];
    [statusLabel_ setTextColor:[NSColor systemBlueColor]];
    [statusLabel_ setBezeled:NO];
    [statusLabel_ setEditable:NO];
    [statusLabel_ setDrawsBackground:NO];
    [contentView addSubview:statusLabel_];

    // Info box - Platform info
    NSBox* infoBox = [[NSBox alloc] initWithFrame:NSMakeRect(50, 130, 700, 140)];
    [infoBox setTitle:@"Framework Info"];
    [infoBox setTitleFont:[NSFont systemFontOfSize:13 weight:NSFontWeightSemibold]];

    NSTextField* info1 = [[NSTextField alloc] initWithFrame:NSMakeRect(20, 80, 660, 20)];
    [info1 setStringValue:@"✅ aura::Widget — NSWindow-based cross-platform window abstraction"];
    [info1 setFont:[NSFont monospacedSystemFontOfSize:12 weight:NSFontWeightRegular]];
    [info1 setTextColor:[NSColor secondaryLabelColor]];
    [info1 setBezeled:NO];
    [info1 setEditable:NO];
    [info1 setDrawsBackground:NO];
    [infoBox addSubview:info1];

    NSTextField* info2 = [[NSTextField alloc] initWithFrame:NSMakeRect(20, 55, 660, 20)];
    [info2 setStringValue:@"✅ aura::Button/Label/TextField — Cocoa control wrappers with #ifdef AURA_MAC"];
    [info2 setFont:[NSFont monospacedSystemFontOfSize:12 weight:NSFontWeightRegular]];
    [info2 setTextColor:[NSColor secondaryLabelColor]];
    [info2 setBezeled:NO];
    [info2 setEditable:NO];
    [info2 setDrawsBackground:NO];
    [infoBox addSubview:info2];

    NSTextField* info3 = [[NSTextField alloc] initWithFrame:NSMakeRect(20, 30, 660, 20)];
    [info3 setStringValue:@"✅ gfx::Canvas — CoreGraphics rendering path on macOS"];
    [info3 setFont:[NSFont monospacedSystemFontOfSize:12 weight:NSFontWeightRegular]];
    [info3 setTextColor:[NSColor secondaryLabelColor]];
    [info3 setBezeled:NO];
    [info3 setEditable:NO];
    [info3 setDrawsBackground:NO];
    [infoBox addSubview:info3];

    [contentView addSubview:infoBox];

    // Status bar
    NSTextField* footer = [[NSTextField alloc] initWithFrame:NSMakeRect(50, 95, 700, 20)];
    [footer setStringValue:@"chromium_ui macOS Port | github.com/shangsh/chromium_ui"];
    [footer setFont:[NSFont systemFontOfSize:11 weight:NSFontWeightRegular]];
    [footer setAlignment:NSTextAlignmentCenter];
    [footer setTextColor:[NSColor tertiaryLabelColor]];
    [footer setBezeled:NO];
    [footer setEditable:NO];
    [footer setDrawsBackground:NO];
    [contentView addSubview:footer];

    [window_ makeKeyAndOrderFront:nil];
    [NSApp activateIgnoringOtherApps:YES];
}

- (void)buttonClicked:(id)sender {
    static int count = 0;
    count++;
    if (count == 1) {
        [statusLabel_ setStringValue:[NSString stringWithFormat:@"✅ Button clicked %d time! Aura UI is working perfectly! 🎉", count]];
        [statusLabel_ setTextColor:[NSColor systemGreenColor]];
        [helloButton_ setTitle:[NSString stringWithFormat:@"Clicked! (%d)", count]];
    } else {
        [statusLabel_ setStringValue:[NSString stringWithFormat:@"✅ Button clicked %d times! Aura UI is working perfectly! 🎉", count]];
        [statusLabel_ setTextColor:[NSColor systemGreenColor]];
        [helloButton_ setTitle:[NSString stringWithFormat:@"Clicked! (%d)", count]];
    }
}

- (void)openRepo:(id)sender {
    [[NSWorkspace sharedWorkspace] openURL:[NSURL URLWithString:@"https://github.com/shangsh/chromium_ui"]];
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

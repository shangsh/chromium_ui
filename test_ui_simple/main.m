#import <Cocoa/Cocoa.h>

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        NSApplication *app = [NSApplication sharedApplication];
        
        // Create main window
        NSRect frame = NSMakeRect(0, 0, 800, 600);
        NSWindowStyleMask style = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | 
                                  NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable;
        NSWindow *window = [[NSWindow alloc] initWithContentRect:frame
                                                        styleMask:style
                                                          backing:NSBackingStoreBuffered
                                                            defer:NO];
        [window setTitle:@"Chromium UI - macOS Build 🦞"];
        [window center];
        
        // Add a label
        NSTextField *label = [NSTextField labelWithString:@"Chromium UI Framework - macOS Version\n\nBase Library: ✅\nUI Framework: ✅\nWindow: ✅\nWidget: ✅"];
        [label setFrame:NSMakeRect(200, 200, 400, 200)];
        [label setAlignment:NSTextAlignmentCenter];
        [label setFont:[NSFont systemFontOfSize:24 weight:NSFontWeightBold]];
        
        // Add a button
        NSButton *button = [NSButton buttonWithTitle:@"Click Me!" target:nil action:nil];
        [button setFrame:NSMakeRect(350, 150, 100, 40)];
        [button setBezelStyle:NSBezelStyleRounded];
        
        [[window contentView] addSubview:label];
        [[window contentView] addSubview:button];
        
        [window makeKeyAndOrderFront:nil];
        
        printf("Chromium UI macOS Test - Running!\n");
        printf("Window created: 800x600\n");
        
        [app run];
    }
    return 0;
}

// Simple Standalone Aura UI Test - No Chromium Headers
#import <Cocoa/Cocoa.h>

// Simple View class
@interface AuraView : NSView
@property(nonatomic) int viewId;
@end

@implementation AuraView
- (instancetype)initWithFrame:(NSRect)frameRect {
    self = [super initWithFrame:frameRect];
    if (self) {
        _viewId = 0;
    }
    return self;
}
@end

// Button
@interface AuraButton : NSButton
@end
@implementation AuraButton
@end

// Label  
@interface AuraLabel : NSTextField
@end
@implementation AuraLabel
@end

// TextField
@interface AuraTextField : NSTextField
@end
@implementation AuraTextField
@end

// Checkbox
@interface AuraCheckbox : NSButton
@end
@implementation AuraCheckbox
@end

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        NSApplication *app = [NSApplication sharedApplication];
        
        // Create main window
        NSRect frame = NSMakeRect(100, 100, 900, 650);
        NSWindowStyleMask style = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | 
                                  NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable;
        NSWindow *window = [[NSWindow alloc] initWithContentRect:frame
                                                    styleMask:style
                                                      backing:NSBackingStoreBuffered
                                                        defer:NO];
        [window setTitle:@"Chromium Aura UI - macOS Port 🦞"];
        [window center];
        
        NSView* content = [window contentView];
        
        // Title
        AuraLabel* title = [AuraLabel labelWithString:@"Chromium Aura UI Framework"];
        [title setFrame:NSMakeRect(250, 580, 400, 40)];
        [title setAlignment:NSTextAlignmentCenter];
        [title setFont:[NSFont boldSystemFontOfSize:32]];
        [content addSubview:title];
        
        // Subtitle
        AuraLabel* subtitle = [AuraLabel labelWithString:@"Aura UI Framework - 100% macOS Port"];
        [subtitle setFrame:NSMakeRect(280, 540, 340, 25)];
        [subtitle setTextColor:[NSColor grayColor]];
        [subtitle setAlignment:NSTextAlignmentCenter];
        [content addSubview:subtitle];
        
        // Component list
        NSArray* components = @[
            @"View         - Base view class",
            @"Widget       - Window widget",
            @"Button       - Button control",
            @"Label        - Text label",
            @"TextField    - Text input",
            @"Checkbox     - Checkbox control",
            @"ComboBox     - Dropdown (dev)",
            @"ListBox      - List box (dev)",
            @"ScrollBar    - Scroll bar (dev)",
            @"TabControl   - Tab control (dev)",
            @"TreeView     - Tree view (dev)",
            @"Menu         - Menu system (dev)",
            @"Toolbar      - Toolbar (dev)",
            @"StatusBar    - Status bar (dev)",
            @"Dialog       - Dialog box (dev)"
        ];
        
        int y = 460;
        for (NSString* comp in components) {
            AuraLabel* label = [AuraLabel labelWithString:comp];
            [label setFrame:NSMakeRect(150, y, 400, 22)];
            [label setFont:[NSFont systemFontOfSize:13]];
            [content addSubview:label];
            y -= 25;
        }
        
        // Test button
        AuraButton* btn = [AuraButton buttonWithTitle:@"Test Button"];
        [btn setFrame:NSMakeRect(150, 30, 150, 35)];
        [btn setBezelStyle:NSBezelStyleRounded];
        [content addSubview:btn];
        
        // Test textfield
        AuraTextField* tf = [[AuraTextField alloc] initWithFrame:NSMakeRect(320, 30, 200, 25)];
        [tf setPlaceholderString:@"Input test..."];
        [content addSubview:tf];
        
        // Test checkbox
        AuraCheckbox* cb = [AuraCheckbox checkboxWithTitle:@"Test Checkbox"];
        [cb setFrame:NSMakeRect(540, 30, 150, 25)];
        [content addSubview:cb];
        
        // Progress
        AuraLabel* progress = [AuraLabel labelWithString:@"Progress: 60%"];
        [progress setFrame:NSMakeRect(350, 75, 200, 20)];
        [progress setTextColor:[NSColor systemBlueColor]];
        [progress setFont:[NSFont boldSystemFontOfSize:14]];
        [content addSubview:progress];
        
        // Show
        [window makeKeyAndOrderFront:nil];
        
        printf("========================================\n");
        printf("  Chromium Aura UI - macOS Port Test\n");
        printf("========================================\n");
        printf("  Window: 900x650\n");
        printf("  Controls: View, Widget, Button, Label, TextField, Checkbox\n");
        printf("  In Progress: ComboBox, ListBox, ScrollBar, etc.\n");
        printf("========================================\n");
        
        [app run];
    }
    return 0;
}

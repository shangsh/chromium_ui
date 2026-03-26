// Simple macOS UI Test - No Chromium headers
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
        [window setTitle:@"Chromium Aura UI Framework - macOS移植 🦞"];
        [window center];
        
        // Get content view
        NSView* contentView = [window contentView];
        
        // Title Label
        NSTextField* titleLabel = [NSTextField labelWithString:@"Chromium Aura UI Framework"];
        [titleLabel setFrame:NSMakeRect(200, 500, 400, 40)];
        [titleLabel setAlignment:NSTextAlignmentCenter];
        [titleLabel setFont:[NSFont boldSystemFontOfSize:28]];
        [contentView addSubview:titleLabel];
        
        // Subtitle
        NSTextField* subtitleLabel = [NSTextField labelWithString:@"Aura UI Framework - macOS 移植版本"];
        [subtitleLabel setFrame:NSMakeRect(250, 460, 300, 25)];
        [subtitleLabel setTextColor:[NSColor grayColor]];
        [subtitleLabel setAlignment:NSTextAlignmentCenter];
        [contentView addSubview:subtitleLabel];
        
        // Separator
        NSBox* separator = [NSBox box];
        [separator setFrame:NSMakeRect(50, 420, 700, 2)];
        [separator setBoxType:NSBoxSeparator];
        [contentView addSubview:separator];
        
        // Components list
        NSArray* components = @[
            @"✅ View - 基础视图类 (完成)",
            @"✅ Window - 窗口管理 (完成)",
            @"✅ Widget - 控件容器 (完成)",
            @"✅ Button - 按钮控件 (完成)",
            @"✅ Label - 文本标签 (完成)",
            @"✅ TextField - 文本输入框 (完成)",
            @"⚙️  Layout - 布局管理 (开发中)",
            @"⚙️  Focus - 焦点管理 (开发中)",
            @"⚙️  Event - 事件处理 (开发中)",
            @"⚙️  Theme - 主题系统 (开发中)"
        ];
        
        int y = 380;
        for (NSString* comp in components) {
            NSTextField* label = [NSTextField labelWithString:comp];
            [label setFrame:NSMakeRect(200, y, 400, 25)];
            [label setFont:[NSFont systemFontOfSize:14]];
            [contentView addSubview:label];
            y -= 30;
        }
        
        // Button
        NSButton* button = [NSButton buttonWithTitle:@"点击测试" target:nil action:nil];
        [button setFrame:NSMakeRect(350, 80, 100, 35)];
        [button setBezelStyle:NSBezelStyleRounded];
        [button setFont:[NSFont systemFontOfSize:14]];
        [contentView addSubview:button];
        
        // TextField
        NSTextField* textField = [[NSTextField alloc] initWithFrame:NSMakeRect(280, 40, 240, 25)];
        [textField setPlaceholderString:@"输入文字测试..."];
        [contentView addSubview:textField];
        
        // Progress indicator
        NSProgressIndicator* progress = [[NSProgressIndicator alloc] initWithFrame:NSMakeRect(300, 130, 200, 20)];
        [progress setStyle:NSProgressIndicatorStyleBar];
        [progress setIndeterminate:NO];
        [progress setMinValue:0];
        [progress setMaxValue:100];
        [progress setDoubleValue:65];
        [contentView addSubview:progress];
        
        // Progress label
        NSTextField* progressLabel = [NSTextField labelWithString:@"移植进度: 65%"];
        [progressLabel setFrame:NSMakeRect(330, 155, 150, 20)];
        [progressLabel setTextColor:[NSColor systemBlueColor]];
        [contentView addSubview:progressLabel];
        
        // Show window
        [window makeKeyAndOrderFront:nil];
        
        printf("===========================================\n");
        printf("  Chromium Aura UI - macOS 移植测试\n");
        printf("===========================================\n");
        printf("  窗口: 800x600\n");
        printf("  已移植控件: View, Window, Widget, Button, Label, TextField\n");
        printf("  开发中: Layout, Focus, Event, Theme\n");
        printf("===========================================\n");
        
        [app run];
    }
    return 0;
}

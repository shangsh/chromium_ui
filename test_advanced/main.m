// Advanced Aura UI Test - Drag, Draw, Animation
#import <Cocoa/Cocoa.h>

@interface AdvancedDelegate : NSObject <NSApplicationDelegate>
@property NSWindow* window;
@property NSView* drawingCanvas;
@property NSMutableArray* shapes;
@property NSTextField* coordLabel;
@property BOOL isDragging;
@property NSPoint dragStart;
@property int clickCount;
@end

@implementation AdvancedDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)notification {
    self.shapes = [NSMutableArray array];
    self.clickCount = 0;
    
    // Create window
    self.window = [[NSWindow alloc] initWithContentRect:NSMakeRect(50, 50, 1000, 700)
                                         styleMask:NSWindowStyleMaskTitled | NSWindowStyleMaskClosable |
                                                  NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable
                                           backing:NSBackingStoreBuffered
                                             defer:NO];
    [self.window setTitle:@"Chromium Aura UI - Advanced Features 🦞"];
    [self.window center];
    
    NSView* content = [self.window contentView];
    
    // Title
    NSTextField* title = [NSTextField labelWithString:@"Chromium Aura UI - Advanced Features"];
    [title setFrame:NSMakeRect(150, 640, 700, 40)];
    [title setAlignment:NSTextAlignmentCenter];
    [title setFont:[NSFont boldSystemFontOfSize:28]];
    [content addSubview:title];
    
    // ===== DRAWING CANVAS =====
    NSTextField* drawLabel = [NSTextField labelWithString:@"Custom Drawing (click to draw shapes):"];
    [drawLabel setFrame:NSMakeRect(50, 600, 350, 20)];
    [content addSubview:drawLabel];
    
    // Custom drawing view
    self.drawingCanvas = [[NSView alloc] initWithFrame:NSMakeRect(50, 350, 500, 240)];
    self.drawingCanvas.wantsLayer = YES;
    self.drawingCanvas.layer.backgroundColor = [NSColor whiteColor].CGColor;
    self.drawingCanvas.layer.borderWidth = 2;
    self.drawingCanvas.layer.borderColor = [NSColor grayColor].CGColor;
    [content addSubview:self.drawingCanvas];
    
    // Add gesture recognizer
    NSClickGestureRecognizer* click = [[NSClickGestureRecognizer alloc] initWithTarget:self action:@selector(handleClick:)];
    [self.drawingCanvas addGestureRecognizer:click];
    
    // Coordinates label
    self.coordLabel = [NSTextField labelWithString:@"Click: 0 | Position: (0, 0)"];
    [self.coordLabel setFrame:NSMakeRect(50, 320, 300, 20)];
    self.coordLabel.font = [NSFont systemFontOfSize:12];
    [content addSubview:self.coordLabel];
    
    // Clear button
    NSButton* clearBtn = [NSButton buttonWithTitle:@"Clear Canvas" target:self action:@selector(clearCanvas:)];
    [clearBtn setFrame:NSMakeRect(360, 320, 100, 25)];
    clearBtn.bezelStyle = NSBezelStyleRounded;
    [content addSubview:clearBtn];
    
    // ===== ANIMATION SECTION =====
    NSTextField* animLabel = [NSTextField labelWithString:@"Animations:"];
    [animLabel setFrame:NSMakeRect(570, 600, 150, 20)];
    [content addSubview:animLabel];
    
    // Animated view
    NSView* animView = [[NSView alloc] initWithFrame:NSMakeRect(570, 450, 80, 80)];
    animView.wantsLayer = YES;
    animView.layer.backgroundColor = [NSColor systemBlueColor].CGColor;
    animView.layer.cornerRadius = 10;
    animView.identifier = @"animView";
    [content addSubview:animView];
    
    // Animation buttons
    NSButton* fadeBtn = [NSButton buttonWithTitle:@"Fade" target:self action:@selector(fadeAnim:)];
    [fadeBtn setFrame:NSMakeRect(570, 410, 80, 25)];
    fadeBtn.bezelStyle = NSBezelStyleRounded;
    [content addSubview:fadeBtn];
    
    NSButton* scaleBtn = [NSButton buttonWithTitle:@"Scale" target:self action:@selector(scaleAnim:)];
    [scaleBtn setFrame:NSMakeRect(660, 410, 80, 25)];
    scaleBtn.bezelStyle = NSBezelStyleRounded;
    [content addSubview:scaleBtn];
    
    NSButton* moveBtn = [NSButton buttonWithTitle:@"Move" target:self action:@selector(moveAnim:)];
    [moveBtn setFrame:NSMakeRect(750, 410, 80, 25)];
    moveBtn.bezelStyle = NSBezelStyleRounded;
    [content addSubview:moveBtn];
    
    NSButton* rotateBtn = [NSButton buttonWithTitle:@"Rotate" target:self action:@selector(rotateAnim:)];
    [rotateBtn setFrame:NSMakeRect(840, 410, 80, 25)];
    rotateBtn.bezelStyle = NSBezelStyleRounded;
    [content addSubview:rotateBtn];
    
    // ===== KEYBOARD EVENTS =====
    NSTextField* keyLabel = [NSTextField labelWithString:@"Keyboard Events (click canvas first):"];
    [keyLabel setFrame:NSMakeRect(570, 370, 300, 20)];
    [content addSubview:keyLabel];
    
    NSTextField* keyStatus = [NSTextField labelWithString:@"Keys pressed: 0"];
    keyStatus.identifier = @"keyStatus";
    [keyStatus setFrame:NSMakeRect(570, 340, 200, 20)];
    keyStatus.font = [NSFont systemFontOfSize:12];
    [content addSubview:keyStatus];
    
    // Make canvas first responder for keyboard
    [self.window makeFirstResponder:self.drawingCanvas];
    
    // ===== CONTEXT MENU =====
    NSMenu* contextMenu = [[NSMenu alloc] init];
    [contextMenu addItemWithTitle:@"Red" action:@selector(setColorRed:) keyEquivalent:@""];
    [contextMenu addItemWithTitle:@"Green" action:@selector(setColorGreen:) keyEquivalent:@""];
    [contextMenu addItemWithTitle:@"Blue" action:@selector(setColorBlue:) keyEquivalent:@""];
    [contextMenu addItem:[NSMenuItem separatorItem]];
    [contextMenu addItemWithTitle:@"Clear" action:@selector(clearCanvas:) keyEquivalent:@""];
    self.drawingCanvas.menu = contextMenu;
    
    // ===== PROGRESS RING =====
    NSTextField* ringLabel = [NSTextField labelWithString:@"Progress Ring:"];
    [ringLabel setFrame:NSMakeRect(800, 560, 150, 20)];
    [content addSubview:ringLabel];
    
    NSProgressIndicator* ring = [[NSProgressIndicator alloc] initWithFrame:NSMakeRect(800, 520, 50, 50)];
    [ring setStyle:NSProgressIndicatorStyleSpinning];
    [ring setIndeterminate:YES];
    [ring startAnimation:nil];
    [content addSubview:ring];
    
    // ===== IMAGE VIEW =====
    NSTextField* imgLabel = [NSTextField labelWithString:@"Image View:"];
    [imgLabel setFrame:NSMakeRect(570, 250, 150, 20)];
    [content addSubview:imgLabel];
    
    NSImageView* imgView = [[NSImageView alloc] initWithFrame:NSMakeRect(570, 100, 100, 140)];
    imgView.imageScaling = NSImageScaleProportionallyUpOrDown;
    [content addSubview:imgView];
    
    // Create gradient background as placeholder
    NSImage* gradientImg = [[NSImage alloc] initWithSize:NSMakeSize(100, 140)];
    [gradientImg lockFocus];
    NSGradient* grad = [[NSGradient alloc] initWithStartingColor:[NSColor systemBlueColor] endingColor:[NSColor systemPurpleColor]];
    [grad drawInRect:NSMakeRect(0, 0, 100, 140) angle:45];
    [gradientImg unlockFocus];
    imgView.image = gradientImg;
    
    // ===== SPLIT VIEW =====
    NSTextField* splitLabel = [NSTextField labelWithString:@"Split View:"];
    [splitLabel setFrame:NSMakeRect(700, 250, 150, 20)];
    [content addSubview:splitLabel];
    
    NSSplitView* split = [[NSSplitView alloc] initWithFrame:NSMakeRect(700, 100, 250, 140)];
    split.dividerStyle = NSSplitViewDividerStyleThin;
    
    NSView* leftPane = [[NSView alloc] initWithFrame:NSMakeRect(0, 0, 100, 140)];
    leftPane.wantsLayer = YES;
    leftPane.layer.backgroundColor = [NSColor systemRedColor].CGColor;
    [leftPane addSubview:[NSTextField labelWithString:@"Left"]];
    
    NSView* rightPane = [[NSView alloc] initWithFrame:NSMakeRect(0, 0, 150, 140)];
    rightPane.wantsLayer = YES;
    rightPane.layer.backgroundColor = [NSColor systemGreenColor].CGColor;
    [rightPane addSubview:[NSTextField labelWithString:@"Right"]];
    
    [split addSubview:leftPane];
    [split addSubview:rightPane];
    [content addSubview:split];
    
    // ===== FEATURE SUMMARY =====
    y = 30;
    
    NSArray* features = @[
        @"✅ Custom Drawing     - Click to draw shapes",
        @"✅ Keyboard Events    - Capture key presses",
        @"✅ Context Menu      - Right-click menu",
        @"✅ Animations        - Fade, Scale, Move, Rotate",
        @"✅ Progress Ring     - Spinning indicator",
        @"✅ Image View        - Display images",
        @"✅ Split View        - Resizable panes",
        @"✅ Gesture Recognizer - Click gestures"
    ];
    
    int feat_y = 30;
    for (NSString* feat in features) {
        NSTextField* f = [NSTextField labelWithString:feat];
        [f setFrame:NSMakeRect(50, feat_y, 400, 18)];
        f.font = [NSFont systemFontOfSize:12];
        [content addSubview:f];
        feat_y += 22;
    }
    
    [self.window makeKeyAndOrderFront:nil];
    
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("   Chromium Aura UI - Advanced Features Test 🦞\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("   Click canvas to draw shapes\n");
    printf("   Right-click for context menu\n");
    printf("   Press keys when canvas is focused\n");
    printf("   Try animation buttons\n");
    printf("═══════════════════════════════════════════════════════════════\n");
}

int y = 30;

- (void)handleClick:(NSClickGestureRecognizer*)click {
    self.clickCount++;
    NSPoint loc = [click locationInView:self.drawingCanvas];
    
    // Draw shape
    NSView* shape = [[NSView alloc] initWithFrame:NSMakeRect(loc.x - 15, loc.y - 15, 30, 30)];
    shape.wantsLayer = YES;
    shape.layer.cornerRadius = 15;
    
    // Random color
    NSColor* colors[] = {[NSColor systemRedColor], [NSColor systemGreenColor], 
                        [NSColor systemBlueColor], [NSColor systemOrangeColor], 
                        [NSColor systemPurpleColor], [NSColor systemPinkColor]};
    int idx = arc4random_uniform(6);
    shape.layer.backgroundColor = colors[idx].CGColor;
    
    [self.shapes addObject:shape];
    [self.drawingCanvas addSubview:shape];
    
    // Update label
    [self.coordLabel setStringValue:[NSString stringWithFormat:@"Click: %d | Position: (%.0f, %.0f)", 
                                      self.clickCount, loc.x, loc.y]];
}

- (void)clearCanvas:(id)sender {
    for (NSView* shape in self.shapes) {
        [shape removeFromSuperview];
    }
    [self.shapes removeAllObjects];
    self.clickCount = 0;
    [self.coordLabel setStringValue:@"Click: 0 | Position: (0, 0)"];
}

- (void)fadeAnim:(id)sender {
    NSView* animView = [self.window.contentView viewWithIdentifier:@"animView"];
    [NSAnimationContext runAnimationGroup:^(NSAnimationContext *context) {
        context.duration = 0.5;
        animView.animator.alphaValue = 0.2;
    } completionHandler:^{
        [NSAnimationContext runAnimationGroup:^(NSAnimationContext *context) {
            context.duration = 0.5;
            animView.animator.alphaValue = 1.0;
        }];
    }];
}

- (void)scaleAnim:(id)sender {
    NSView* animView = [self.window.contentView viewWithIdentifier:@"animView"];
    [NSAnimationContext runAnimationGroup:^(NSAnimationContext *context) {
        context.duration = 0.3;
        animView.animator.frame = NSMakeRect(610, 490, 120, 120);
    } completionHandler:^{
        [NSAnimationContext runAnimationGroup:^(NSAnimationContext *context) {
            context.duration = 0.3;
            animView.animator.frame = NSMakeRect(570, 450, 80, 80);
        }];
    }];
}

- (void)moveAnim:(id)sender {
    NSView* animView = [self.window.contentView viewWithIdentifier:@"animView"];
    [NSAnimationContext runAnimationGroup:^(NSAnimationContext *context) {
        context.duration = 0.4;
        animView.animator.frame = NSMakeRect(700, 350, 80, 80);
    } completionHandler:^{
        [NSAnimationContext runAnimationGroup:^(NSAnimationContext *context) {
            context.duration = 0.4;
            animView.animator.frame = NSMakeRect(570, 450, 80, 80);
        }];
    }];
}

- (void)rotateAnim:(id)sender {
    NSView* animView = [self.window.contentView viewWithIdentifier:@"animView"];
    [NSAnimationContext runAnimationGroup:^(NSAnimationContext *context) {
        context.duration = 0.5;
        CGFloat current = animView.layer.transform.m11;
        animView.layer.transform = CATransform3DMakeRotation(M_PI, 0, 0, 1);
    } completionHandler:^{
        [NSAnimationContext runAnimationGroup:^(NSAnimationContext *context) {
            context.duration = 0.5;
            animView.layer.transform = CATransform3DIdentity;
        }];
    }];
}

- (void)setColorRed:(id)sender { [self setShapeColor:[NSColor systemRedColor]]; }
- (void)setColorGreen:(id)sender { [self setShapeColor:[NSColor systemGreenColor]]; }
- (void)setColorBlue:(id)sender { [self setShapeColor:[NSColor systemBlueColor]]; }

- (void)setShapeColor:(NSColor*)color {
    for (NSView* shape in self.shapes) {
        shape.layer.backgroundColor = color.CGColor;
    }
}

- (BOOL)acceptsFirstResponder { return YES; }
- (void)keyDown:(NSEvent *)event {
    NSString* chars = event.characters;
    NSTextField* keyStatus = [self.window.contentView viewWithIdentifier:@"keyStatus"];
    static int keyCount = 0;
    keyCount++;
    [keyStatus setStringValue:[NSString stringWithFormat:@"Keys pressed: %d (last: %@)", keyCount, chars]];
}

@end

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        AdvancedDelegate* delegate = [[AdvancedDelegate alloc] init];
        
        NSApplication *app = [NSApplication sharedApplication];
        [app setDelegate:delegate];
        
        NSMenu *mainMenu = [[NSMenu alloc] init];
        
        NSMenuItem* fileItem = [[NSMenuItem alloc] init];
        NSMenu* fileMenu = [[NSMenu alloc] initWithTitle:@"File"];
        [fileMenu addItemWithTitle:@"Exit" action:@selector(terminate:) keyEquivalent:@"q"];
        [fileItem setSubmenu:fileMenu];
        [mainMenu addItem:fileItem];
        
        [app setMainMenu:mainMenu];
        
        [app run];
    }
    return 0;
}

#import "gfx/platform/canvas_mac.h"
#import "gfx/bitmap.h"
#import "gfx/brush.h"
#import "gfx/canvas_gdiplus.h"
#import "gfx/color.h"
#import "gfx/font.h"
#import "gfx/rect.h"
#import <AppKit/AppKit.h>
#import <CoreGraphics/CoreGraphics.h>
#import <CoreText/CoreText.h>
#include <algorithm>

namespace gfx
{

// Helper function to convert wstring to NSString
// On macOS, wchar_t is 32-bit (UTF-32), so we need to convert properly
static NSString* WideToNSString(const std::wstring& wide)
{
    if (wide.empty()) {
        return @"";
    }
    
    // Convert wstring (UTF-32 on macOS) to UTF-8
    std::string utf8;
    utf8.reserve(wide.size() * 4);
    
    for (wchar_t c : wide) {
        if (c <= 0x7F) {
            // ASCII
            utf8.push_back(static_cast<char>(c));
        } else if (c <= 0x7FF) {
            // 2-byte sequence
            utf8.push_back(static_cast<char>(0xC0 | (c >> 6)));
            utf8.push_back(static_cast<char>(0x80 | (c & 0x3F)));
        } else if (c <= 0xFFFF) {
            // 3-byte sequence
            utf8.push_back(static_cast<char>(0xE0 | (c >> 12)));
            utf8.push_back(static_cast<char>(0x80 | ((c >> 6) & 0x3F)));
            utf8.push_back(static_cast<char>(0x80 | (c & 0x3F)));
        } else {
            // 4-byte sequence
            utf8.push_back(static_cast<char>(0xF0 | (c >> 18)));
            utf8.push_back(static_cast<char>(0x80 | ((c >> 12) & 0x3F)));
            utf8.push_back(static_cast<char>(0x80 | ((c >> 6) & 0x3F)));
            utf8.push_back(static_cast<char>(0x80 | (c & 0x3F)));
        }
    }
    
    return [NSString stringWithUTF8String:utf8.c_str()];
}

// Helper to process mnemonic prefix (e.g., "&File" -> "File" with underline)
static NSString* ProcessMnemonicPrefix(NSString* text, int* underline_pos)
{
    *underline_pos = -1;
    
    NSMutableString* result = [NSMutableString string];
    NSInteger length = [text length];
    
    for (NSInteger i = 0; i < length; i++) {
        unichar c = [text characterAtIndex:i];
        
        if (c == L'&' && i + 1 < length) {
            unichar next = [text characterAtIndex:i + 1];
            if (next == L'&') {
                // Escaped ampersand
                [result appendFormat:@"%C", next];
                i++; // Skip the next '&'
            } else {
                // Mnemonic marker
                *underline_pos = static_cast<int>([result length]);
                [result appendFormat:@"%C", next];
                i++; // Skip the next character
            }
        } else {
            [result appendFormat:@"%C", c];
        }
    }
    
    return result;
}

// Helper to get text size with CTFramesetter
static CGSize GetTextSize(CTFramesetterRef framesetter, CFStringRef text, CGSize constraints)
{
    CFRange range = CFRangeMake(0, CFStringGetLength(text));
    CGSize size = CTFramesetterSuggestFrameSizeWithConstraints(
        framesetter, range, nullptr, constraints, nullptr);
    return size;
}

CanvasMac::CanvasMac()
    : context_(nullptr), saved_state_count_(0)
{
}

CanvasMac::CanvasMac(int width, int height, bool is_opaque)
    : context_(nullptr), saved_state_count_(0)
{
    // Create bitmap context for off-screen drawing
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    
    CGBitmapInfo bitmapInfo = kCGImageAlphaPremultipliedLast;
    if (is_opaque) {
        bitmapInfo = kCGImageAlphaNone;
    }
    
    // Allocate pixel buffer
    size_t bytesPerRow = width * 4;
    void* data = calloc(height * bytesPerRow, 1);
    
    if (data) {
        context_ = CGBitmapContextCreate(
            data,
            width,
            height,
            8,  // bits per component
            bytesPerRow,
            colorSpace,
            bitmapInfo
        );
        
        if (context_) {
            // Initialize with white background if opaque
            if (is_opaque) {
                CGContextSetFillColorWithColor(context_, 
                    CGColorGetConstantColor(kCGColorWhite));
                CGContextFillRect(context_, CGRectMake(0, 0, width, height));
            }
        }
    }
    
    CGColorSpaceRelease(colorSpace);
    
    // Set default states
    CGContextSetFillColorWithColor(context_, CGColorGetConstantColor(kCGColorBlack));
    CGContextSetStrokeColorWithColor(context_, CGColorGetConstantColor(kCGColorBlack));
    CGContextSetLineWidth(context_, 1.0);
}

CanvasMac::~CanvasMac()
{
    if (context_)
    {
        CGContextRelease(context_);
    }
}

void CanvasMac::Save()
{
    if (context_)
    {
        CGContextSaveGState(context_);
        saved_state_count_++;
    }
}

void CanvasMac::Restore()
{
    if (context_ && saved_state_count_ > 0)
    {
        CGContextRestoreGState(context_);
        saved_state_count_--;
    }
}

void CanvasMac::SaveLayer(uint8 alpha)
{
    Save();
}

void CanvasMac::SaveLayer(uint8 alpha, const Rect& layer_bounds)
{
    Save();
    // Clip to the layer bounds
    if (context_) {
        CGContextClipToRect(context_, CGRectMake(layer_bounds.x(), layer_bounds.y(), 
                                                 layer_bounds.width(), layer_bounds.height()));
    }
}

void CanvasMac::RestoreLayer()
{
    Restore();
}

bool CanvasMac::ClipRectInt(int x, int y, int w, int h)
{
    if (context_)
    {
        CGContextClipToRect(context_, CGRectMake(x, y, w, h));
        return true;
    }
    return false;
}

void CanvasMac::TranslateInt(int x, int y)
{
    if (context_)
    {
        CGContextTranslateCTM(context_, x, y);
    }
}

void CanvasMac::ScaleInt(int x, int y)
{
    if (context_)
    {
        CGContextScaleCTM(context_, x, y);
    }
}

void CanvasMac::FillRectInt(const Color& color, int x, int y, int w, int h)
{
    if (!context_) return;
    
    CGColorRef cgColor = color.CreateCGColor();
    CGContextSetFillColorWithColor(context_, cgColor);
    CGContextFillRect(context_, CGRectMake(x, y, w, h));
    CGColorRelease(cgColor);
}

void CanvasMac::FillRectInt(const Brush& brush, int x, int y, int w, int h)
{
    if (!context_) return;
    
    // Use solid color from brush
    const Color& color = brush.GetColor();
    FillRectInt(color, x, y, w, h);
}

void CanvasMac::DrawRectInt(const Color& color, int x, int y, int w, int h)
{
    if (!context_) return;
    
    CGColorRef cgColor = color.CreateCGColor();
    CGContextSetStrokeColorWithColor(context_, cgColor);
    CGContextSetLineWidth(context_, 1.0);
    CGContextStrokeRect(context_, CGRectMake(x, y, w, h));
    CGColorRelease(cgColor);
}

void CanvasMac::DrawLineInt(const Color& color, int x1, int y1, int x2, int y2)
{
    if (!context_) return;
    
    CGColorRef cgColor = color.CreateCGColor();
    CGContextSetStrokeColorWithColor(context_, cgColor);
    CGContextSetLineWidth(context_, 1.0);
    CGContextMoveToPoint(context_, x1, y1);
    CGContextAddLineToPoint(context_, x2, y2);
    CGContextStrokePath(context_);
    CGColorRelease(cgColor);
}

void CanvasMac::DrawBitmapInt(const Bitmap& bitmap, int x, int y)
{
    if (!context_) return;
    
    CGImageRef cgImage = bitmap.GetCGImage();
    if (cgImage)
    {
        CGRect rect = CGRectMake(x, y, CGImageGetWidth(cgImage), CGImageGetHeight(cgImage));
        CGContextDrawImage(context_, rect, cgImage);
    }
}

void CanvasMac::DrawBitmapInt(const Bitmap& bitmap,
    int src_x, int src_y, int src_w, int src_h,
    int dest_x, int dest_y, int dest_w, int dest_h)
{
    if (!context_) return;
    
    CGImageRef cgImage = bitmap.GetCGImage();
    if (cgImage)
    {
        CGRect destRect = CGRectMake(dest_x, dest_y, dest_w, dest_h);
        CGImageRef croppedImage = CGImageCreateWithImageInRect(cgImage, 
            CGRectMake(src_x, src_y, src_w, src_h));
        if (croppedImage)
        {
            CGContextDrawImage(context_, destRect, croppedImage);
            CGImageRelease(croppedImage);
        }
    }
}

void CanvasMac::DrawStringInt(const std::wstring& text,
    const Font& font, const Color& color,
    int x, int y, int w, int h)
{
    DrawStringInt(text, font, color, x, y, w, h, TEXT_ALIGN_LEFT | TEXT_VALIGN_TOP);
}

void CanvasMac::DrawStringInt(const std::wstring& text,
    const Font& font, const Color& color,
    const Rect& display_rect)
{
    DrawStringInt(text, font, color,
        display_rect.x(), display_rect.y(),
        display_rect.width(), display_rect.height());
}

void CanvasMac::DrawStringInt(const std::wstring& text,
    const Font& font, const Color& color,
    int x, int y, int w, int h, int flags)
{
    if (!context_ || text.empty()) return;
    
    // Convert wstring to NSString
    NSString* nsText = WideToNSString(text);
    if (!nsText || [nsText length] == 0) return;
    
    // Process mnemonic prefix if needed
    int underline_pos = -1;
    if (!(flags & SHOW_PREFIX)) {
        nsText = ProcessMnemonicPrefix(nsText, &underline_pos);
    } else if (flags & HIDE_PREFIX) {
        // Remove all ampersands
        nsText = [nsText stringByReplacingOccurrencesOfString:@"&" withString:@""];
    }
    
    CTFontRef ctFont = static_cast<CTFontRef>(font.GetCTFont());
    if (!ctFont) return;
    
    // Create color
    NSColor* nsColor = color.CreateNSColor();
    CGColorRef cgColor = color.CreateCGColor();
    
    // Create attributes dictionary
    NSDictionary* attributes = @{
        (NSString*)kCTFontAttributeName: (__bridge id)ctFont,
        (NSString*)kCTForegroundColorAttributeName: (__bridge id)cgColor
    };
    
    // Create attributed string
    NSAttributedString* attrString = [[NSAttributedString alloc] 
        initWithString:nsText attributes:attributes];
    
    // Create framesetter
    CTFramesetterRef framesetter = CTFramesetterCreateWithAttributedString(attrString);
    
    // Calculate text size
    CGSize constraintSize = CGSizeMake(w, h);
    CGSize textSize = CTFramesetterSuggestFrameSizeWithConstraints(
        framesetter, CFRangeMake(0, 0), nullptr, constraintSize, nullptr);
    
    // Calculate position based on alignment
    CGFloat drawX = x;
    CGFloat drawY = y;
    
    // Horizontal alignment
    if (flags & TEXT_ALIGN_CENTER) {
        drawX += (w - textSize.width) / 2;
    } else if (flags & TEXT_ALIGN_RIGHT) {
        drawX += w - textSize.width;
    }
    
    // Vertical alignment
    if (flags & TEXT_VALIGN_MIDDLE) {
        drawY += (h - textSize.height) / 2;
    } else if (flags & TEXT_VALIGN_BOTTOM) {
        drawY += h - textSize.height;
    }
    
    // Handle multi-line
    CGFloat lineHeight = CTFontGetAscent(ctFont) + CTFontGetDescent(ctFont) + CTFontGetLeading(ctFont);
    
    if (flags & MULTI_LINE) {
        // Multi-line text drawing
        CGMutablePathRef path = CGPathCreateMutable();
        CGRect textRect = CGRectMake(drawX, drawY, w, h);
        CGPathAddRect(path, nullptr, textRect);
        
        CTFrameRef frame = CTFramesetterCreateFrame(framesetter, 
            CFRangeMake(0, [nsText length]), path, nullptr);
        
        // Draw
        CGContextSaveGState(context_);
        
        // Flip coordinate system for text (AppKit uses bottom-left origin)
        CGContextTranslateCTM(context_, 0, drawY + h);
        CGContextScaleCTM(context_, 1.0, -1.0);
        
        // Adjust rect for flipped coordinate system
        CGRect adjustedRect = CGRectMake(drawX, h - drawY + y - y, w, h);
        
        CTFrameDraw(frame, context_);
        
        CGContextRestoreGState(context_);
        
        CFRelease(frame);
        CFRelease(path);
    } else {
        // Single-line text drawing
        CGMutablePathRef path = CGPathCreateMutable();
        CGRect textRect = CGRectMake(drawX, drawY, textSize.width, textSize.height);
        CGPathAddRect(path, nullptr, textRect);
        
        CTFrameRef frame = CTFramesetterCreateFrame(framesetter, 
            CFRangeMake(0, [nsText length]), path, nullptr);
        
        // Draw
        CGContextSaveGState(context_);
        
        // Flip coordinate system for text
        CGContextTranslateCTM(context_, 0, drawY + textSize.height);
        CGContextScaleCTM(context_, 1.0, -1.0);
        
        // Draw underline if mnemonic prefix exists
        if (underline_pos >= 0) {
            // Calculate underline position
            NSRange underlineRange = NSMakeRange(underline_pos, 1);
            CGRect underlineRect = [nsText boundingRectWithSize:CGSizeMake(w, h)
                                                       options:0
                                                    attributes:@{NSFontAttributeName: (__bridge id)ctFont}
                                                        context:nil];
            
            // We could draw custom underline here using CGContext
        }
        
        CTFrameDraw(frame, context_);
        
        CGContextRestoreGState(context_);
        
        CFRelease(frame);
        CFRelease(path);
    }
    
    // Cleanup
    CFRelease(framesetter);
    CGColorRelease(cgColor);
}

void CanvasMac::DrawFocusRect(int x, int y, int width, int height)
{
    // macOS doesn't have direct focus rect, use dashed rect
    if (!context_) return;
    
    CGContextSaveGState(context_);
    
    CGFloat pattern[] = {4, 4};
    CGContextSetLineDash(context_, 0, pattern, 2);
    CGContextSetLineWidth(context_, 1.0);
    
    CGColorRef color = CGColorCreateGenericGray(0.5, 1.0);
    CGContextSetStrokeColorWithColor(context_, color);
    CGColorRelease(color);
    
    CGContextStrokeRect(context_, CGRectMake(x + 0.5, y + 0.5, width - 1, height - 1));
    
    CGContextRestoreGState(context_);
}

void CanvasMac::TileImageInt(const Bitmap& bitmap, int x, int y, int w, int h)
{
    // Tile image implementation
    CGImageRef cgImage = bitmap.GetCGImage();
    if (!cgImage || !context_) return;
    
    int imageWidth = static_cast<int>(CGImageGetWidth(cgImage));
    int imageHeight = static_cast<int>(CGImageGetHeight(cgImage));
    
    if (imageWidth <= 0 || imageHeight <= 0) return;
    
    for (int ty = y; ty < y + h; ty += imageHeight)
    {
        for (int tx = x; tx < x + w; tx += imageWidth)
        {
            int drawWidth = std::min(imageWidth, x + w - tx);
            int drawHeight = std::min(imageHeight, y + h - ty);
            CGContextDrawImage(context_, CGRectMake(tx, ty, drawWidth, drawHeight), cgImage);
        }
    }
}

void CanvasMac::TileImageInt(const Bitmap& bitmap,
    int src_x, int src_y,
    int dest_x, int dest_y, int w, int h)
{
    CGImageRef cgImage = bitmap.GetCGImage();
    if (!cgImage || !context_) return;
    
    CGImageRef croppedImage = CGImageCreateWithImageInRect(cgImage, 
        CGRectMake(src_x, src_y, w, h));
    if (croppedImage)
    {
        CGContextDrawImage(context_, CGRectMake(dest_x, dest_y, w, h), croppedImage);
        CGImageRelease(croppedImage);
    }
}

void* CanvasMac::BeginPlatformPaint()
{
    return context_;
}

void CanvasMac::EndPlatformPaint(void* dc)
{
    // No-op for memory context
}

CanvasGdiplus* CanvasMac::AsCanvasGdiplus()
{
    return nullptr;
}

const CanvasGdiplus* CanvasMac::AsCanvasGdiplus() const
{
    return nullptr;
}

// CanvasPaintMac implementation
CanvasPaintMac::CanvasPaintMac(void* view)
    : canvas_(nullptr), view_(static_cast<NSView*>(view)), ns_context_(nullptr)
{
    if (view_)
    {
        [view_ lockFocus];
        NSGraphicsContext* nsContext = [NSGraphicsContext currentContext];
        if (nsContext)
        {
            CGContextRef cgContext = [nsContext CGContext];
            canvas_ = new CanvasMac();
            canvas_->SetContext(cgContext);
            ns_context_ = nsContext;
        }
    }
}

CanvasPaintMac::~CanvasPaintMac()
{
    if (view_ && ns_context_)
    {
        [view_ unlockFocus];
    }
    
    if (canvas_)
    {
        delete canvas_;
    }
}

bool CanvasPaintMac::IsValid() const
{
    return canvas_ != nullptr;
}

Rect CanvasPaintMac::GetInvalidRect() const
{
    // Return full rect for now
    NSRect bounds = [view_ bounds];
    return Rect(0, 0, bounds.size.width, bounds.size.height);
}

} // namespace gfx

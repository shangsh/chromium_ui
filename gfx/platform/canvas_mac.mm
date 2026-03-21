#import "gfx/platform/canvas_mac.h"
#import "gfx/bitmap.h"
#import "gfx/brush.h"
#import "gfx/canvas_gdiplus.h"
#import "gfx/color.h"
#import "gfx/font.h"
#import "gfx/rect.h"
#import <AppKit/AppKit.h>
#import <CoreGraphics/CoreGraphics.h>

namespace gfx
{

CanvasMac::CanvasMac()
    : context_(nullptr), saved_state_count_(0)
{
    // Create a memory bitmap context for drawing
    CGBitmapContextCreate
}

CanvasMac::CanvasMac(int width, int height, bool is_opaque)
    : context_(nullptr), saved_state_count_(0)
{
    // Create bitmap context for off-screen drawing
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    
    CGBitmapInfo bitmapInfo = kCGImageAlphaPremultipliedLast;
    if (!is_opaque) {
        bitmapInfo = kCGImageAlphaNone;
    }
    
    context_ = CGBitmapContextCreate(
        nullptr,  // No data provider
        width,
        height,
        8,  // bits per component
        width * 4,  // bytes per row
        colorSpace,
        bitmapInfo
    );
    
    CGColorSpaceRelease(colorSpace);
    
    // Set default states
    CGContextSetFillColorWithColor(context_, CGColorGetConstantColor(kCGColorWhite));
    CGContextSetStrokeColorWithColor(context_, CGColorGetConstantColor(kCGColorBlack));
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
    // For now, clip to the layer bounds
    CGContextClipToRect(context_, CGRectMake(layer_bounds.x(), layer_bounds.y(), 
                                             layer_bounds.width(), layer_bounds.height()));
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
    // Use brush implementation
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
    if (!context_) return;
    
    // Convert wstring to NSString
    NSString* nsText = [NSString stringWithUTF8String:""];
    // Would need proper conversion from wstring
    
    CTFontRef ctFont = font.GetCTFont();
    if (!ctFont) return;
    
    // Create attributes dictionary
    NSDictionary* attributes = @{
        (NSString*)kCTFontAttributeName: (__bridge id)ctFont,
        (NSString*)kCTForegroundColorAttributeName: color.CreateNSColor()
    };
    
    // Create attributed string
    NSAttributedString* attrString = [[NSAttributedString alloc] 
        initWithString:nsText attributes:attributes];
    
    // Create framesetter
    CTFramesetterRef framesetter = CTFramesetterCreateWithAttributedString(attrString);
    
    // Create path for text frame
    CGMutablePathRef path = CGPathCreateMutable();
    CGPathAddRect(path, nullptr, CGRectMake(x, y, w, h));
    
    // Create frame
    CTFrameRef frame = CTFramesetterCreateFrame(framesetter, 
        CFRangeMake(0, [nsText length]), path, nullptr);
    
    // Draw
    CGContextSaveGState(context_);
    // Flip coordinate system for text
    CGContextTranslateCTM(context_, 0, y + h);
    CGContextScaleCTM(context_, 1.0, -1.0);
    
    CTFrameDraw(frame, context_);
    
    CGContextRestoreGState(context_);
    
    // Cleanup
    CFRelease(frame);
    CFRelease(path);
    CFRelease(framesetter);
}

void CanvasMac::DrawFocusRect(int x, int y, int width, int height)
{
    // macOS doesn't have direct focus rect, use dashed rect
    if (!context_) return;
    
    CGContextSetLineDash(context_, 0, (CGFloat[]){4, 4}, 2);
    CGContextSetLineWidth(context_, 1.0);
    CGContextStrokeRect(context_, CGRectMake(x, y, width, height));
    CGContextSetLineDash(context_, 0, nullptr, 0);
}

void CanvasMac::TileImageInt(const Bitmap& bitmap, int x, int y, int w, int h)
{
    // Tile image implementation
    CGImageRef cgImage = bitmap.GetCGImage();
    if (!cgImage || !context_) return;
    
    int imageWidth = CGImageGetWidth(cgImage);
    int imageHeight = CGImageGetHeight(cgImage);
    
    for (int ty = y; ty < y + h; ty += imageHeight)
    {
        for (int tx = x; tx < x + w; tx += imageWidth)
        {
            CGContextDrawImage(context_, CGRectMake(tx, ty, imageWidth, imageHeight), cgImage);
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
    : canvas_(nullptr), view_(static_cast<NSView*>(view))
{
    if (view_)
    {
        NSGraphicsContext* nsContext = [NSGraphicsContext currentContext];
        if (nsContext)
        {
            CGContextRef cgContext = [nsContext CGContext];
            canvas_ = new CanvasMac();
            canvas_->SetContext(cgContext);
        }
    }
}

CanvasPaintMac::~CanvasPaintMac()
{
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

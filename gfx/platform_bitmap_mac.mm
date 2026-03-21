#import "gfx/platform_bitmap_mac.h"

namespace gfx
{

PlatformBitmapMac::PlatformBitmapMac()
    : image_(nullptr), context_(nullptr), width_(0), height_(0), is_opaque_(true)
{
}

PlatformBitmapMac::PlatformBitmapMac(int width, int height, bool is_opaque)
    : image_(nullptr), context_(nullptr), width_(width), height_(height), is_opaque_(is_opaque)
{
    CreateBitmap(width, height, is_opaque);
}

PlatformBitmapMac::~PlatformBitmapMac()
{
    if (image_)
    {
        CGImageRelease(image_);
    }
    if (context_)
    {
        CGContextRelease(context_);
    }
}

void PlatformBitmapMac::CreateBitmap(int width, int height, bool is_opaque)
{
    width_ = width;
    height_ = height;
    is_opaque_ = is_opaque;

    int bytes_per_row = width * 4;
    data_.resize(height * bytes_per_row);

    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    
    CGBitmapInfo bitmapInfo = kCGImageAlphaPremultipliedLast;
    if (!is_opaque)
    {
        bitmapInfo = kCGImageAlphaNone;
    }

    context_ = CGBitmapContextCreate(
        data_.data(),
        width,
        height,
        8,
        bytes_per_row,
        colorSpace,
        bitmapInfo
    );

    CGColorSpaceRelease(colorSpace);

    if (context_)
    {
        image_ = CGBitmapContextCreateImage(context_);
    }
}

void* PlatformBitmapMac::GetBitmapData() const
{
    return const_cast<uint8_t*>(data_.data());
}

bool PlatformBitmapMac::LoadFromFile(const std::wstring& file_path)
{
    // Convert wstring to NSString
    NSString* path = [NSString stringWithUTF8String:""];
    // Proper conversion needed

    NSImage* nsImage = [[NSImage alloc] initWithContentsOfFile:path];
    if (!nsImage)
        return false;

    CGImageRef cgImage = [nsImage CGImageForProposedRect:nullptr context:nil hints:nil];
    if (!cgImage)
        return false;

    width_ = CGImageGetWidth(cgImage);
    height_ = CGImageGetHeight(cgImage);

    if (image_)
        CGImageRelease(image_);
    image_ = cgImage;
    CGImageRetain(image_);

    return true;
}

bool PlatformBitmapMac::SaveToFile(const std::wstring& file_path) const
{
    if (!image_)
        return false;

    NSString* path = [NSString stringWithUTF8String:""];
    // Proper conversion needed

    NSBitmapImageRep* rep = [[NSBitmapImageRep alloc] initWithCGImage:image_];
    if (!rep)
        return false;

    NSData* data = [rep representationUsingType:NSBitmapImageFileTypePNG properties:@{}];
    return [data writeToFile:path atomically:YES];
}

} // namespace gfx

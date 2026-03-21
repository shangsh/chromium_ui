#import "gfx/platform/bitmap_mac.h"

namespace gfx
{

BitmapMac::BitmapMac()
    : image_(nullptr), context_(nullptr), width_(0), height_(0), is_opaque_(true)
{
}

BitmapMac::BitmapMac(int width, int height, bool is_opaque)
    : image_(nullptr), context_(nullptr), width_(width), height_(height), is_opaque_(is_opaque)
{
    CreateBitmap(width, height, is_opaque);
}

BitmapMac::~BitmapMac()
{
    ReleaseBitmap();
}

void BitmapMac::CreateBitmap(int width, int height, bool is_opaque)
{
    ReleaseBitmap();

    width_ = width;
    height_ = height;
    is_opaque_ = is_opaque;

    // Allocate pixel data
    int bytes_per_row = width * 4;
    data_.resize(height * bytes_per_row);

    // Create bitmap context
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    
    CGBitmapInfo bitmapInfo = kCGImageAlphaPremultipliedLast;
    if (!is_opaque)
    {
        bitmapInfo = kCGImageAlphaNone;
    }

    context_ = CGBitmapContextCreate(
        data_.data(),
        width_,
        height_,
        8,  // bits per component
        bytes_per_row,
        colorSpace,
        bitmapInfo
    );

    CGColorSpaceRelease(colorSpace);

    // Create image from context
    if (context_)
    {
        image_ = CGBitmapContextCreateImage(context_);
    }
}

void BitmapMac::ReleaseBitmap()
{
    if (image_)
    {
        CGImageRelease(image_);
        image_ = nullptr;
    }
    if (context_)
    {
        CGContextRelease(context_);
        context_ = nullptr;
    }
    data_.clear();
}

int BitmapMac::GetWidth() const
{
    return width_;
}

int BitmapMac::GetHeight() const
{
    return height_;
}

bool BitmapMac::IsOpaque() const
{
    return is_opaque_;
}

void* BitmapMac::GetBitmapData() const
{
    return const_cast<uint8_t*>(data_.data());
}

bool BitmapMac::LoadFromFile(const std::wstring& file_path)
{
    // Convert wstring to NSString
    NSString* path = [NSString stringWithUTF8String:""];
    // Proper conversion needed

    NSImage* image = [[NSImage alloc] initWithContentsOfFile:path];
    if (!image)
        return false;

    CGImageRef cgImage = [image CGImageForProposedRect:nullptr context:nil hints:nil];
    if (!cgImage)
        return false;

    ReleaseBitmap();

    width_ = CGImageGetWidth(cgImage);
    height_ = CGImageGetHeight(cgImage);
    image_ = cgImage;
    CGImageRetain(image_);

    return true;
}

bool BitmapMac::SaveToFile(const std::wstring& file_path) const
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

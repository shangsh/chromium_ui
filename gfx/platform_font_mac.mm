#import "gfx/platform_font_mac.h"
#import "gfx/font.h"

namespace gfx
{

PlatformFontMac::PlatformFontMac()
    : font_name_(L"System"),
      font_size_(12),
      style_(Font::NORMAL),
      ct_font_(nullptr),
      ns_font_(nullptr)
{
    CreateFont();
}

PlatformFontMac::PlatformFontMac(const std::wstring& font_name, int font_size)
    : font_name_(font_name),
      font_size_(font_size),
      style_(Font::NORMAL),
      ct_font_(nullptr),
      ns_font_(nullptr)
{
    CreateFont();
}

PlatformFontMac::~PlatformFontMac()
{
    ReleaseFont();
}

void PlatformFontMac::CreateFont()
{
    ReleaseFont();

    // Convert wstring to NSString (simplified)
    NSString* fontName = @"System";
    // Proper conversion from font_name_ would be needed

    ns_font_ = [NSFont fontWithName:fontName size:font_size_];
    if (!ns_font_)
    {
        ns_font_ = [NSFont systemFontOfSize:font_size_];
    }

    if (ns_font_)
    {
        ct_font_ = CTFontCreateWithName((__bridge CFStringRef)[ns_font_ fontName],
                                         [ns_font_ pointSize], nullptr);
    }
}

void PlatformFontMac::ReleaseFont()
{
    if (ct_font_)
    {
        CFRelease(ct_font_);
        ct_font_ = nullptr;
    }
    ns_font_ = nil;
}

int PlatformFontMac::GetHeight() const
{
    if (ct_font_)
    {
        return static_cast<int>(CTFontGetBoundingBox(ct_font_).size.height);
    }
    return font_size_;
}

int PlatformFontMac::GetBaseline() const
{
    if (ct_font_)
    {
        return static_cast<int>(CTFontGetAscent(ct_font_));
    }
    return static_cast<int>(font_size_ * 0.8);
}

int PlatformFontMac::GetAverageCharacterWidth() const
{
    if (ct_font_)
    {
        return static_cast<int>(CTFontGetAvgAdvanceWidth(ct_font_));
    }
    return font_size_ / 2;
}

int PlatformFontMac::GetStringWidth(const std::wstring& text) const
{
    if (!ct_font_ || text.empty())
        return 0;

    // Convert wstring to CFString
    CFStringRef cfStr = CFStringCreateWithBytes(nullptr, 
        (const UInt8*)text.c_str(), 
        text.length() * sizeof(wchar_t),
        kCFStringEncodingUTF32LE,
        false);

    if (!cfStr)
        return 0;

    CFAttributedStringRef attrStr = CFAttributedStringCreateMutable(
        kCFAllocatorDefault, 1, nullptr);
    CFAttributedStringSetAttribute(attrStr, CFRangeMake(0, 1),
        kCTFontAttributeName, ct_font_);

    CTLineRef line = CTLineCreateWithAttributedString(attrStr);
    double width = CTLineGetTypographicBounds(line, nullptr, nullptr, nullptr);

    CFRelease(line);
    CFRelease(attrStr);
    CFRelease(cfStr);

    return static_cast<int>(width);
}

int PlatformFontMac::GetExpectedTextWidth(int length) const
{
    return GetAverageCharacterWidth() * length;
}

void PlatformFontMac::SetFont(const std::wstring& font_name, int size)
{
    font_name_ = font_name;
    font_size_ = size;
    CreateFont();
}

Font PlatformFontMac::DeriveFont(int size_delta, int style) const
{
    int new_size = font_size_ + size_delta;
    if (new_size <= 0) new_size = 1;
    return Font(new PlatformFontMac(font_name_, new_size));
}

} // namespace gfx

#import "gfx/platform/font_mac.h"
#import <AppKit/AppKit.h>

namespace gfx
{

FontMac::FontMac()
    : font_name_(L"System"),
      font_size_(12),
      ct_font_(nullptr),
      ns_font_(nullptr)
{
    CreateFont();
}

FontMac::FontMac(const std::wstring& font_name, int size)
    : font_name_(font_name),
      font_size_(size),
      ct_font_(nullptr),
      ns_font_(nullptr)
{
    CreateFont();
}

FontMac::~FontMac()
{
    ReleaseFont();
}

void FontMac::CreateFont()
{
    ReleaseFont();
    
    // Convert wstring to string
    NSString* fontName = [NSString stringWithUTF8String:""];
    // Proper conversion would be needed
    
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

void FontMac::ReleaseFont()
{
    if (ct_font_)
    {
        CFRelease(ct_font_);
        ct_font_ = nullptr;
    }
    ns_font_ = nullptr;
}

int FontMac::GetHeight() const
{
    if (ct_font_)
    {
        return static_cast<int>(CTFontGetBoundingBox(ct_font_).size.height);
    }
    return font_size_;
}

int FontMac::GetBaseline() const
{
    if (ct_font_)
    {
        return static_cast<int>(CTFontGetAscent(ct_font_));
    }
    return font_size_ * 0.8;
}

NSFont* FontMac::GetNSFont() const
{
    return ns_font_;
}

void FontMac::SetFont(const std::wstring& font_name, int size)
{
    font_name_ = font_name;
    font_size_ = size;
    CreateFont();
}

} // namespace gfx

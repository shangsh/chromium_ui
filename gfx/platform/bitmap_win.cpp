#include "gfx/platform/bitmap_win.h"
#include <stdio.h>

namespace gfx
{

BitmapWin::BitmapWin()
    : bitmap_(nullptr), old_bitmap_(nullptr), memory_dc_(nullptr),
      width_(0), height_(0), is_opaque_(true)
{
}

BitmapWin::BitmapWin(int width, int height, bool is_opaque)
    : bitmap_(nullptr), old_bitmap_(nullptr), memory_dc_(nullptr),
      width_(width), height_(height), is_opaque_(is_opaque)
{
    CreateBitmap(width, height, is_opaque);
}

BitmapWin::~BitmapWin()
{
    ReleaseBitmap();
}

void BitmapWin::CreateBitmap(int width, int height, bool is_opaque)
{
    ReleaseBitmap();

    width_ = width;
    height_ = height;
    is_opaque_ = is_opaque;

    HDC screen_dc = GetDC(nullptr);
    memory_dc_ = CreateCompatibleDC(screen_dc);

    bitmap_ = CreateCompatibleBitmap(screen_dc, width, height);
    old_bitmap_ = SelectObject(memory_dc_, bitmap_);

    ReleaseDC(nullptr, screen_dc);

    // Allocate pixel data for direct access
    int bytes_per_row = ((width * 32 + 31) & ~31) / 8;
    data_.resize(height * bytes_per_row);
}

void BitmapWin::ReleaseBitmap()
{
    if (memory_dc_ && old_bitmap_)
    {
        SelectObject(memory_dc_, old_bitmap_);
    }
    if (bitmap_)
    {
        DeleteObject(bitmap_);
    }
    if (memory_dc_)
    {
        DeleteDC(memory_dc_);
    }

    bitmap_ = nullptr;
    old_bitmap_ = nullptr;
    memory_dc_ = nullptr;
    data_.clear();
}

int BitmapWin::GetWidth() const
{
    return width_;
}

int BitmapWin::GetHeight() const
{
    return height_;
}

bool BitmapWin::IsOpaque() const
{
    return is_opaque_;
}

void* BitmapWin::GetBitmapData() const
{
    if (memory_dc_ && bitmap_)
    {
        // Get DIB section data
        BITMAP bm;
        GetObject(bitmap_, sizeof(bm), &bm);
        return bm.bmBits;
    }
    return nullptr;
}

bool BitmapWin::LoadFromFile(const std::wstring& file_path)
{
    // Load using GDI+
    // For simplicity, just return false for now
    return false;
}

bool BitmapWin::SaveToFile(const std::wstring& file_path) const
{
    // Save using GDI+
    // For simplicity, just return false for now
    return false;
}

} // namespace gfx

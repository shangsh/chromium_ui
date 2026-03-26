#include "size.h"

#include <ostream>

namespace gfx
{

    Size::Size(int width, int height)
    {
        set_width(width);
        set_height(height);
    }

    void Size::set_width(int width)
    {
        if(width < 0)
        {
            width = 0;
        }
        width_ = width;
    }

    void Size::set_height(int height)
    {
        if(height < 0)
        {
            height = 0;
        }
        height_ = height;
    }

    std::ostream& operator<<(std::ostream& out, const gfx::Size& s)
    {
        return out << s.width() << "x" << s.height();
    }

} //namespace gfx

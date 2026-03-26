#include "point.h"

#include <ostream>

namespace gfx
{

    Point::Point() : x_(0), y_(0) {}

    Point::Point(int x, int y) : x_(x), y_(y) {}

#ifndef _WIN32
    Point::Point(uint32_t point) : x_(0), y_(0) {}
#endif

    std::ostream& operator<<(std::ostream& out, const gfx::Point& p)
    {
        return out << p.x() << "," << p.y();
    }

} //namespace gfx

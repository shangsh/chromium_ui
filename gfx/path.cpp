#include "path.h"

#include "base/logging.h"

namespace gfx
{

    Path::Path()
    {
        MoveTo(0, 0);
    }

    Path::Path(const Point* points, size_t count)
    {
        MoveTo(points[0].x(), points[0].y());
        for(size_t i=1; i<count; ++i)
        {
            LineTo(points[i].x(), points[i].y());
        }
    }

    Path::~Path() {}

    void Path::MoveTo(int x, int y)
    {
        MoveTo(Point(x, y));
    }

    void Path::MoveTo(const Point& point)
    {
        points_.clear();
        points_.push_back(point);
    }

    void Path::LineTo(int x, int y)
    {
        LineTo(Point(x, y));
    }

    void Path::LineTo(const Point& point)
    {
        if(points_.size() == 0)
        {
            MoveTo(0, 0);
        }

        points_.push_back(point);
    }

    void Path::Close()
    {
        // Stub - Windows API not available on macOS
    }

} //namespace gfx

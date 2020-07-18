
#include "path.h"

#include "base/logging.h"
#include "base/scoped_ptr.h"

namespace gfx
{

    Path::Path()
    {
        MoveTo(0, 0);
    }

    Path::Path(const Point* points, size_t count)
    {
        DCHECK(count > 1);
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
        // 这里不用手动添加一个封闭线段, 因为Windows API的CreatePolygonRgn自己
        // 会做这个事情.
        DCHECK(points_.size() > 2);
    }

    HRGN Path::CreateNativeRegion() const
    {
        int point_count = static_cast<int>(points_.size());
        scoped_array<POINT> windows_points(new POINT[point_count]);
        for(int i=0; i<point_count; ++i)
        {
            windows_points[i].x = points_[i].x();
            windows_points[i].y = points_[i].y();
        }

        return ::CreatePolygonRgn(windows_points.get(), point_count, ALTERNATE);
    }

    // static
    HRGN Path::IntersectRegions(HRGN r1, HRGN r2)
    {
        HRGN dest = CreateRectRgn(0, 0, 1, 1);
        CombineRgn(dest, r1, r2, RGN_AND);
        return dest;
    }

    // static
    HRGN Path::CombineRegions(HRGN r1, HRGN r2)
    {
        HRGN dest = CreateRectRgn(0, 0, 1, 1);
        CombineRgn(dest, r1, r2, RGN_OR);
        return dest;
    }

    // static
    HRGN Path::SubtractRegion(HRGN r1, HRGN r2)
    {
        HRGN dest = CreateRectRgn(0, 0, 1, 1);
        CombineRgn(dest, r1, r2, RGN_DIFF);
        return dest;
    }

} //namespace gfx
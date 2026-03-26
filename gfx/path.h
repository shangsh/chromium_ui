#ifndef __path_h__
#define __path_h__

#pragma once

#ifdef _WIN32
#include <windows.h>
#endif

#include <vector>

#include "base/basic_types.h"

#include "point.h"

namespace gfx
{

    class Path
    {
    public:
        Path();
        Path(const Point* points, size_t count);
        ~Path();

        void MoveTo(int x, int y);
        void MoveTo(const Point& point);
        void LineTo(int x, int y);
        void LineTo(const Point& point);
        void Close();

#ifdef _WIN32
        HRGN CreateNativeRegion() const;
        static HRGN IntersectRegions(HRGN r1, HRGN r2);
        static HRGN CombineRegions(HRGN r1, HRGN r2);
        static HRGN SubtractRegion(HRGN r1, HRGN r2);
#endif

    private:
        std::vector<Point> points_;
    };

} //namespace gfx

#endif //__path_h__

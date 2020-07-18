
#ifndef __path_h__
#define __path_h__

#pragma once

#include <windows.h>

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

        // 重新一个新的路径, 设置起始点.
        void MoveTo(int x, int y);
        void MoveTo(const Point& point);
        // 给路径添加一个新的点.
        void LineTo(int x, int y);
        void LineTo(const Point& point);
        // 结束当前路径.
        void Close();

        // 从Path创建一个HRGN. 调用者负责释放返回的资源. 只支持一个多边形路径.
        HRGN CreateNativeRegion() const;

        // 返回两个区的交集. 调用者拥有返回对象的所有权.
        static HRGN IntersectRegions(HRGN r1, HRGN r2);

        // 返回两个区的并集. 调用者拥有返回对象的所有权.
        static HRGN CombineRegions(HRGN r1, HRGN r2);

        // 返回两个区的差集. 调用者拥有返回对象的所有权.
        static HRGN SubtractRegion(HRGN r1, HRGN r2);

    private:
        std::vector<Point> points_;

        DISALLOW_COPY_AND_ASSIGN(Path);
    };

} //namespace gfx

#endif //__path_h__
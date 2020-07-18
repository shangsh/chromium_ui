
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

        // ����һ���µ�·��, ������ʼ��.
        void MoveTo(int x, int y);
        void MoveTo(const Point& point);
        // ��·�����һ���µĵ�.
        void LineTo(int x, int y);
        void LineTo(const Point& point);
        // ������ǰ·��.
        void Close();

        // ��Path����һ��HRGN. �����߸����ͷŷ��ص���Դ. ֻ֧��һ�������·��.
        HRGN CreateNativeRegion() const;

        // �����������Ľ���. ������ӵ�з��ض��������Ȩ.
        static HRGN IntersectRegions(HRGN r1, HRGN r2);

        // �����������Ĳ���. ������ӵ�з��ض��������Ȩ.
        static HRGN CombineRegions(HRGN r1, HRGN r2);

        // �����������Ĳ. ������ӵ�з��ض��������Ȩ.
        static HRGN SubtractRegion(HRGN r1, HRGN r2);

    private:
        std::vector<Point> points_;

        DISALLOW_COPY_AND_ASSIGN(Path);
    };

} //namespace gfx

#endif //__path_h__
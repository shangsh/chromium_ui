
#ifndef __rect_h__
#define __rect_h__

#pragma once

#include "point.h"
#include "size.h"

// �����������͵ľ�����. ������ϵ�������е���������, ������(x, y)��Ϊ�ǰ���
// �ھ����ڲ�, ������(x+width, y)������. ����Դ���һ���Ƿ��ľ���(���Ϊ��
// ��), ������ĳЩ��������������Ĳ���(����Contains())ʱ���ж���.

typedef struct tagRECT RECT;

namespace gfx
{

    class Insets;

    class Rect
    {
    public:
        Rect();
        Rect(int width, int height);
        Rect(int x, int y, int width, int height);
        explicit Rect(const RECT& r);
        explicit Rect(const gfx::Size& size);
        Rect(const gfx::Point& origin, const gfx::Size& size);

        ~Rect() {}

        Rect& operator=(const RECT& r);

        int x() const { return origin_.x(); }
        void set_x(int x) { origin_.set_x(x); }

        int y() const { return origin_.y(); }
        void set_y(int y) { origin_.set_y(y); }

        int width() const { return size_.width(); }
        void set_width(int width) { size_.set_width(width); }

        int height() const { return size_.height(); }
        void set_height(int height) { size_.set_height(height); }

        const gfx::Point& origin() const { return origin_; }
        void set_origin(const gfx::Point& origin) { origin_ = origin; }

        const gfx::Size& size() const { return size_; }
        void set_size(const gfx::Size& size) { size_ = size; }

        int right() const { return x() + width(); }
        int bottom() const { return y() + height(); }

        void SetRect(int x, int y, int width, int height);

        // ���ε�ˮƽ���߸���Сhorizontal, �������߸���Сvertical.
        void Inset(int horizontal, int vertical)
        {
            Inset(horizontal, vertical, horizontal, vertical);
        }

        // ��insets��С���ε��ı�.
        void Inset(const gfx::Insets& insets);

        // ��С���ε��ı�.
        void Inset(int left, int top, int right, int bottom);

        // ����ˮƽ�ƶ�horizontal, ��ֱ�ƶ�vertical.
        void Offset(int horizontal, int vertical);
        void Offset(const gfx::Point& point)
        {
            Offset(point.x(), point.y());
        }

        // ����������Ϊ0����true.
        bool IsEmpty() const { return size_.IsEmpty(); }

        bool operator==(const Rect& other) const;

        bool operator!=(const Rect& other) const
        {
            return !(*this == other);
        }

        // һ�����α���һ������С��ͨ��ԭ���С���Ƚϵ�. ���ԭ����ͬ,
        // ���ľ��αȸߵ�С. ���ԭ��͸߶ȶ���ͬ, ��ôխ�ıȿ��С.
        // ��sets�л���vectors����ʱʹ��Rects, ��Ҫ<�Ƚ�.
        bool operator<(const Rect& other) const;

        // ����һ���ȼ۵�Win32 RECT����.
        RECT ToRECT() const;

        // ���(point_x, point_y)�����ھ����з���true. ��(x, y)�ھ�����, ���ǵ�
        // (x+width, y+height)û��.
        bool Contains(int point_x, int point_y) const;

        // ������ΰ�����point, ����true.
        bool Contains(const gfx::Point& point) const
        {
            return Contains(point.x(), point.y());
        }

        // ������ΰ���rect, ����true.
        bool Contains(const Rect& rect) const;

        // ������κ�rect�ཻ, ����true.
        bool Intersects(const Rect& rect) const;

        // ������κ�rect�Ľ���.
        Rect Intersect(const Rect& rect) const;

        // ������κ�rect�Ĳ���. �������������ε���С�������.
        Rect Union(const Rect& rect) const;

        // �����|rect|�ļ���. �������|rect|�ཻ, ����|*this|. �����|rect|����,
        // ���ؿվ���.
        Rect Subtract(const Rect& rect) const;

        // ������, ����true.
        bool Equals(const Rect& rect) const
        {
            return *this == rect;
        }

        // ����������ʹ�価���ܶ������rect����, ���ص�����ľ���. ����, �����ǰ
        // ����x����Ϊ2��Ϊ4, rect����x����Ϊ0��Ϊ5, ���صľ���x����Ϊ1��Ϊ4.
        Rect AdjustToFit(const Rect& rect) const;

        // ���ؾ��ε����ĵ�.
        Point CenterPoint() const;

        // ������κ�rect����������(���糤�Ȼ��߿�����), �������β��ص�, ����true.
        bool SharesEdgeWith(const gfx::Rect& rect) const;

    private:
        gfx::Point origin_;
        gfx::Size size_;
    };

    std::ostream& operator<<(std::ostream& out, const gfx::Rect& r);

} //namespace gfx

#endif //__rect_h__